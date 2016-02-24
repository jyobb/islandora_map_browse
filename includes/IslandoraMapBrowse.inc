<?php

/**
 * @file
 * This file contains two sections.  The first consists of two functions view_all_results 
 * and prepareStaticMapGEOJSONConfig.
 * These are used to generate a static file and view a static map. Loading more than a 
 * few thousand points dynamically is too
 * slow so the full map browse is static.
 *
 * The remainder of the file is a class that extends IslandoraSolrResults creating the dynamic map views.
 */


module_load_include('inc', 'islandora_solr_search', 'IslandoraSolrResults');


/**
 * This function is used to create a static display of points
 * It finds that static map json file, addes the javascript to the page
 * and sends the json.
 */
function view_all_results() {
  $cwd = getcwd();
  $path = drupal_get_path('module', 'islandora_map_browse');
  $dataPath = $cwd . "/" . $path . "/data/AllGEOLocated.json";
  if (file_exists($dataPath) == 1) {
    $response_data = file_get_contents($dataPath);
    $response_data_d = json_decode($response_data, TRUE);
    // Grab configuration information.
    $InfoForJS['js_library'] = variable_get('js_library');
    $InfoForJS['baseUrl'] = variable_get('map_browse_fedora_base');
    $InfoForJS['fedoraPrefix'] = variable_get('map_browse_fedora_prefix');
    $InfoForJS['fedoraSuffix'] = variable_get('map_browse_fedora_suffix');
    $jsInfo = json_encode($InfoForJS);

    // Send javascript to page.
    drupal_add_js(array('islandora_map_browse_settings_jsInfo' => $jsInfo), 'setting');
    drupal_add_js(drupal_get_path('module', 'islandora_map_browse') . '/js/open_map_GEOJSON_location_config.js', 'file');

    // Add leaflet libraries.
    $leaflet_path = libraries_get_path('leaflet');
    if ($leaflet_path != FALSE) {
      drupal_add_js($leaflet_path . '/leaflet.js', 'file');
      drupal_add_css($leaflet_path . '/leaflet.css', 'module', 'all', 'false');
    }
    else {
      watchdog("islandora_map_view", "Leaflet Library not found");
    }

    $cluster_path = libraries_get_path('Leaflet.markercluster-leaflet-0.7');
    if ($cluster_path != FALSE) {
      // Add clustering.
      drupal_add_css($cluster_path . '/dist/MarkerCluster.css', 'module', 'all', 'false');
      drupal_add_css($cluster_path . '/dist/MarkerCluster.Default.css', 'module', 'all', 'false');
      drupal_add_js($cluster_path . '/dist/leaflet.markercluster.js');
    }
    else {
      watchdog("islandora_map_view", "Leaflet Cluster Library not found");
    }
    $results = prepareStaticMapGEOJSONConfig($response_data_d);
    return $results;
  }
}


/**
 *
 */
function prepareStaticMapGEOJSONConfig ($response_data) {

  $uniqueItems = 0;
  $mods_location = variable_get('map_browse_coord_field');
  $map_centre = explode(",", variable_get('map_browse_map_centre'));
  $centre_lat = trim($map_centre[0]);
  $centre_lon = trim($map_centre[1]);

  $displayArr = explode("|", variable_get('map_browse_return_fields'));
  foreach($displayArr as $v){
    $nameField = explode(",", $v);
    $field = trim ($nameField[1]);
    $displayFields[$field] = trim($nameField[0]); 
  }

  /*****Parse for leaflet *******************/
  $leafletData = array();
  foreach ($response_data['objects'] as $object_result) {

    $doc = $object_result['solr_doc'];
    $thumb = $object_result['thumbnail_url'];
    $obj = $object_result['object_url'];
    // Need to put a check in here to avoid the undefined index error.
    if (isset($doc[$mods_location]) && $doc[$mods_location] != '') {
      $parts = explode(",", $doc[$mods_location][0]);
      // Check to see if the location is in the correct format.  If not report to error log.
      if (!isset($parts[1]) || !isset($parts[1])) {
        watchdog("mods_location bad format", "See: " . $doc['PID']);
      }
      else {
        $lat = floatval($parts[0]);
        $lon = floatval($parts[1]);

        /* Check to see if the record 'doc' has any of the fields configured for display
         * Add those that exist and ignore others
         */
        $confFields = '';
        $uniqueItems = $uniqueItems + 1;
        foreach ($displayFields as $k => $v) {
	//Check the document to make sure the display item exists
          if (array_key_exists($k, $doc) && $doc[$k][0] != '') {
            if (strcmp($confFields, '') == 0) {
              $confFields = '<p>' . $v . ': ' . $doc[$k][0] . '</p>';
            }
            else {
              $confFields = '<p>' . $v . ': ' . $doc[$k][0] . '</p>' . $confFields;
            }
          }
        }

        //$title = $doc['dc.title'][0];
	$solrTitle = variable_get('map_browse_title_field');
        $title = $doc[$solrTitle][0];
        $baseURL = variable_get('map_browse_fedora_base');
        $contentString = '<h2><a target="_blank" href="' . $baseURL . '/' . $obj . '"/>' . $title . '</a></h2><img src="' . $baseURL . '/' . $thumb . '">' . $confFields;

        if (array_key_exists("$lat" . "$lon", $leafletData)) {
          $leafletData["$lat" . "$lon"]["content"] = $leafletData["$lat" . "$lon"]["content"] . $contentString;
        }
        else {
          $leafletData["$lat" . "$lon"] = array("lat" => $lat, "lon" => $lon, "content" => $contentString);
        }
      }
    }
  }

  $pinInfo['uniqueItems'] = $uniqueItems;
  $pinInfo['uniqueLocations'] = count($leafletData);

  // Encode everything that goes to JS.
  $jsPinInfo = json_encode($pinInfo);
  drupal_add_js(array('islandora_map_browse_settings_centreLat' => $centre_lat), 'setting');
  drupal_add_js(array('islandora_map_browse_settings_centreLon' => $centre_lon), 'setting');
  drupal_add_js(array('islandora_map_browse_settings_pininfo' => $jsPinInfo), 'setting');

  drupal_add_js(array('islandora_map_browse_settings_leafletpins' => $leafletData), 'setting');

  $output = '<div id="info_canvas"></div><div id="map_canvas" style="width:940px; height:800px;></div>';

  return $output;

}


/**
 * Extention of IslandoraSolrResults for templating purposes.
 * This overrides the displayResults function to provide an alternate display type.
 */
class IslandoraMapBrowse extends IslandoraSolrResults {

  /**
   * Outputs results basically in the normal way, but with thumbnails pulled
   * from the Fedora repository.
   *
   * @param $results
   *   A solr resultset object.
   *
   * @return
   *   html output for the resultset. Note: we currently create this
   *   output manually, should refactor to use drupal forms api.
   */


  /**
   * @param $results
   *   A solr resultset object.
   *
   * @return
   *   html output for the resultset.
   */
  function mapGEOJSONResults($solrQueryProcessor) {
    $page = 0;
    $increment = 20;
    $lowerlimit = ((($page + 1) * $increment) - $increment);

    //Get user configurable fields
    $configFields = "";
    $displayArr = explode("|", variable_get('map_browse_return_fields'));
    foreach($displayArr as $v){
      $nameField = explode(",", $v);
      $configFields .= trim ($nameField[1]) . ', ';
    }

    // Set up the variables that get passed to javascript.
    $this->prepareGEOJSONVariablesConfig($solrQueryProcessor);
    $response_data = $solrQueryProcessor->islandoraSolrResult['response'];

    // The fields added here are required.
    $fl_params = "PID, RELS_EXT_hasModel_uri_ms, " . $configFields . variable_get('map_browse_title_field') . ", " . variable_get('map_browse_coord_field');

    $solrQueryProcessor->solrParams['fl'] = array($fl_params);

    // This section resets the limits on the query and re-executes.
    $solrQueryProcessor->solrStart = $lowerlimit;
    $solrQueryProcessor->solrLimit = $increment;
    $solrQueryProcessor->resetResults();
    $solrQueryProcessor->executeQuery(FALSE);
    $response_data = $solrQueryProcessor->islandoraSolrResult['response'];
    $out = $this->prepareSolrResultsGEOJSONConfig($response_data);
    return $out;
  }

  /**
   * Fuction called by the secondary display profile to create map
   * Uses prepareGEOJSONVariablesConfig
   * Uses prepareSolrResultsGEOJSONConfig.
   */
  function mapGEOJSONAll($solrQueryProcessor) {

    $page = 0;

    if (array_key_exists('generate', $_GET)) {
      $gen = $_GET['generate'];
    }

    $cwd = getcwd();
    $path = drupal_get_path('module', 'islandora_map_browse');
    $dataPath = $cwd . "/" . $path . "/data/AllGEOLocated.json";
    $increment = 13000;
    $lowerlimit = ((($page + 1) * $increment) - $increment);

    // Set up the variables that get passed to javascript.
    $this->prepareGEOJSONVariablesConfig($solrQueryProcessor);
    $response_data = $solrQueryProcessor->islandoraSolrResult['response'];

    $configFields = "";
    $displayArr = explode("|", variable_get('map_browse_return_fields'));
    foreach($displayArr as $v){
      $nameField = explode(",", $v);
      $configFields .= trim ($nameField[1]) . ', ';
    }

    // The fields added here are required.
    $fl_params = "PID, RELS_EXT_hasModel_uri_ms, " . $configFields . variable_get('map_browse_title_field') . ", " . variable_get('map_browse_coord_field');
    $solrQueryProcessor->solrParams['fl'] = array($fl_params);

    // This section resets the limits on the query and re-executes.
    $solrQueryProcessor->solrStart = $lowerlimit;
    $solrQueryProcessor->solrLimit = $increment;
    $solrQueryProcessor->resetResults();
    $solrQueryProcessor->executeQuery(FALSE);
    $response_data = $solrQueryProcessor->islandoraSolrResult['response'];

    if (strcmp($gen, 'yes') == 0) {
      file_put_contents($dataPath, json_encode($response_data));
    }

    $out = $this->prepareSolrResultsGEOJSONConfig($response_data);
    return $out;

  }


  /**
   * @param $solrQueryProcessor
   *   A solr query object
   *
   * @return nothing
   * Passes information needed by javascript to process results
   */
  function prepareGEOJSONVariablesConfig($solrQueryProcessor) {
    // Grab information from query.
    $InfoForJS['qt'] = $solrQueryProcessor->solrQuery;
    $InfoForJS['numFound'] = $solrQueryProcessor->islandoraSolrResult['response']['numFound'];
    $InfoForJS['rows'] = $solrQueryProcessor->solrLimit;

    // Grab configuration information.
    $InfoForJS['js_library'] = variable_get('js_library');
    $InfoForJS['baseUrl'] = variable_get('map_browse_fedora_base');
    $InfoForJS['fedoraPrefix'] = variable_get('map_browse_fedora_prefix');
    $InfoForJS['fedoraSuffix'] = variable_get('map_browse_fedora_suffix');
    $jsInfo = json_encode($InfoForJS);

    // Send javascript to page.
    drupal_add_js(array('islandora_map_browse_settings_jsInfo' => $jsInfo), 'setting');
    drupal_add_js(drupal_get_path('module', 'islandora_map_browse') . '/js/open_map_GEOJSON_location_config.js', 'file');

    // Add leaflet libraries.
    $leaflet_path = libraries_get_path('leaflet');
    if ($leaflet_path != FALSE) {
      drupal_add_js($leaflet_path . '/leaflet.js', 'file');
      drupal_add_css($leaflet_path . '/leaflet.css', 'module', 'all', 'false');
    }
    else {
      watchdog("islandora_map_view", "Leaflet Library not found");
    }

    $cluster_path = libraries_get_path('Leaflet.markercluster-leaflet-0.7');
    if ($cluster_path != FALSE) {
      // Add clustering.
      drupal_add_css($cluster_path . '/dist/MarkerCluster.css', 'module', 'all', 'false');
      drupal_add_css($cluster_path . '/dist/MarkerCluster.Default.css', 'module', 'all', 'false');
      drupal_add_js($cluster_path . '/dist/leaflet.markercluster.js');
    }
    else {
      watchdog("islandora_map_view", "Leaflet Cluster Library not found");
    }

  }



  /**
   * @params $solrResultSet
   *
   * @return HTML output
   *
   * This functions dedups the results of the query and send them to javascript
   * it also passes some information about the result sets
   */
  function prepareSolrResultsGEOJSONConfig ($response_data) {

    $uniqueItems = 0;
    $mods_location = variable_get('map_browse_coord_field');
    $map_centre = explode(",", variable_get('map_browse_map_centre'));
    $centre_lat = trim($map_centre[0]);
    $centre_lon = trim($map_centre[1]);

  $displayArr = explode("|", variable_get('map_browse_return_fields'));
  foreach($displayArr as $v){
    $nameField = explode(",", $v);
    $field = trim ($nameField[1]);
    $displayFields[$field] = trim($nameField[0]); 
  }

    //Parse for leaflet
    $leafletData = array();
    foreach ($response_data['objects'] as $object_result) {
      $doc = $object_result['solr_doc'];
      $thumb = $object_result['thumbnail_url'];
      $obj = $object_result['object_url'];

      // Need to put a check in here to avoid the undefined index error.
      if (isset($doc[$mods_location]) && $doc[$mods_location] != '') {
        $parts = explode(",", $doc[$mods_location][0]);
        // Check to see if the location is in the correct format.  If not report to error log.
        if (!isset($parts[1]) || !isset($parts[1])) {
          // watchdog("mods_location bad format", "See: " .$doc['PID']);.
        }
        else {
          $lat = floatval($parts[0]);
          $lon = floatval($parts[1]);

          // Check to see if the record 'doc' has any of the fields configured for display
          $confFields = '';
          $uniqueItems = $uniqueItems + 1;
          foreach ($displayFields as $k => $v) {
            //$tv = trim($v);
            if (array_key_exists($k, $doc) && $doc[$k][0] != '') {
              if (strcmp($confFields, '') == 0) {
                $confFields = '<p>' . $v . ': ' . $doc[$k][0] . '</p>';
              }
              else {
                $confFields = '<p>' . $v . ': ' . $doc[$k][0] . '</p>' . $confFields;
              }
            }
          }

          //$title = $doc['dc.title'][0];
        $solrTitle = variable_get('map_browse_title_field');
        $title = $doc[$solrTitle][0];
          $baseURL = variable_get('map_browse_fedora_base');
          $contentString = '<h2><a target="_blank" href="' . $baseURL . '/' . $obj . '"/>' . $title . '</a></h2><img src="' . $baseURL . '/' . $thumb . '">' . $confFields;
          if (array_key_exists("$lat" . "$lon", $leafletData)) {
            $leafletData["$lat" . "$lon"]["content"] = $leafletData["$lat" . "$lon"]["content"] . $contentString;
          }
          else {
            $leafletData["$lat" . "$lon"] = array("lat" => $lat, "lon" => $lon, "content" => $contentString);
          }
        }
      }
      else {
        if (strcmp($doc["RELS_EXT_hasModel_uri_ms"]['0'], "info:fedora/islandora:collectionCModel") != 0) {
        }
      }
    }

    $pinInfo['uniqueItems'] = $uniqueItems;
    $pinInfo['uniqueLocations'] = count($leafletData);

    if (strpos($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip') !== FALSE) {
      ob_start('ob_gzhandler');
    }

    // Encode everything that goes to JS.
    $jsPinInfo = json_encode($pinInfo);
    drupal_add_js(array('islandora_map_browse_settings_centreLat' => $centre_lat), 'setting');
    drupal_add_js(array('islandora_map_browse_settings_centreLon' => $centre_lon), 'setting');
    drupal_add_js(array('islandora_map_browse_settings_pininfo' => $jsPinInfo), 'setting');

    drupal_add_js(array('islandora_map_browse_settings_leafletpins' => $leafletData), 'setting');

    $output = '<div id="info_canvas" style="width:800px; height:20px;"></div><div id="map_canvas" style="width:960px; height:800px; float: left;"></div>';

    return $output;

  }

}

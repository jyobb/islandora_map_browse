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

    $cluster_path = libraries_get_path('Leaflet.markercluster-master');
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
  // $withVal = 0;
  // $withoutVal = 0;.
  $uniqueItems = 0;

  $mods_location = variable_get('map_browse_coord_field');
  $map_centre = explode(",", variable_get('map_browse_map_centre'));
  $centre_lat = trim($map_centre[0]);
  $centre_lon = trim($map_centre[1]);

  /***************************************************************/
  /*****Parse for leaflet *******************/
  $leafletData = array();
  foreach ($response_data['objects'] as $object_result) {

    $doc = $object_result['solr_doc'];
    $thumb = $object_result['thumbnail_url'];
    $obj = $object_result['object_url'];
    //$coordinatesLatLon = NULL;
    // Need to put a check in here to avoid the undefined index error.
    if (isset($doc[$mods_location]) && $doc[$mods_location] != '') {
      // $withVal++;.
      $parts = explode(",", $doc[$mods_location][0]);
      // Check to see if the location is in the correct format.  If not report to error log.
      if (!isset($parts[1]) || !isset($parts[1])) {
        watchdog("mods_location bad format", "See: " . $doc['PID']);
      }
      else {

        $lat = floatval($parts[0]);
        $lon = floatval($parts[1]);

        $displayFields = explode(",", variable_get('map_browse_return_fields'));
        //$tdisplayFields = array();

        // Check to see if the record 'doc' has any of the fields configured for display
        // For now hard code in the proper field names.  Maybe get this a configuration option or grab from
        // the solr module.
        $displayNames = array("dc.contributor" => "Contributor", "mods_location_physicalLocation_ms" => "Physical Location", "dc.publisher" => "Publisher");
        $confFields = '';
        $uniqueItems = $uniqueItems + 1;
        //foreach ($displayFields as $k => $v) {
        foreach ($displayFields as $v) {
          $tv = trim($v);
          //$tdisplayFields[$tv] = $tv;
          if (array_key_exists($tv, $doc) && $doc[$tv][0] != '') {
            //$props[$tv] = $doc[$tv];
            if (strcmp($confFields, '') == 0) {
              $confFields = '<p>' . $displayNames[$tv] . ': ' . $doc[$tv][0] . '</p>';
            }
            else {
              $confFields = '<p>' . $displayNames[$tv] . ': ' . $doc[$tv][0] . '</p>' . $confFields;
            }
          }
        }

        $title = $doc['dc.title'][0];
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
        // $withoutVal++;.
      }
    }
  }

  $pinInfo['uniqueItems'] = $uniqueItems;
  $pinInfo['uniqueLocations'] = count($leafletData);

  // var_dump($_SERVER);
  if (strpos($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip') !== FALSE) {
    // Echo "found zip";
    //   ob_start(ob_gzhandler);
  }
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
    $url_args = array();
    $url_args = parse_str(parse_url($_SERVER['HTTP_REFERER'], PHP_URL_QUERY));
    $size = '';
    // drupal_set_message(var_dump($_GET));
    $increment = 20;
    $lowerlimit = ((($page + 1) * $increment) - $increment);

    // Set up the variables that get passed to javascript.
    $this->prepareGEOJSONVariablesConfig($solrQueryProcessor);

    // $queryInfoForJS = array();
    $response_data = $solrQueryProcessor->islandoraSolrResult['response'];

    // Dump the following into drupal set message to see what your dealing with var_dump to debug.
    $res = $solrQueryProcessor;

    // The fields added here are required.
    $fl_params = "PID, RELS_EXT_hasModel_uri_ms, " . variable_get('map_browse_return_fields') . ", " . variable_get('map_browse_title_field') . ", " . variable_get('map_browse_coord_field');
    // RELS_EXT_hasModel_uri_ms, mods_location_url_ms,
    // var_dump($fl_params);
    $solrQueryProcessor->solrParams['fl'] = array($fl_params);

    // This section resets the limits on the query and re-executes.
    $upperlimit = $solrQueryProcessor->islandoraSolrResult['response']['numFound'];

    $solrQueryProcessor->solrStart = $lowerlimit;
    $solrQueryProcessor->solrLimit = $increment;

    $solrQueryProcessor->resetResults();
    $solrQueryProcessor->executeQuery(FALSE);
    $response_data = $solrQueryProcessor->islandoraSolrResult['response'];

    $out = $this->prepareSolrResultsGEOJSONConfig($response_data);

    return $out;

    // $cont = render($out,NULL);
    // return ctools_modal_render("title", $cont);
  }//End of mapPageResults

  /**
   * Fuction called by the secondary display profile to create map
   * Uses prepareGEOJSONVariablesConfig
   * Uses prepareSolrResultsGEOJSONConfig.
   */
  function mapGEOJSONAll($solrQueryProcessor) {

    $page = 0;
    $url_args = array();

    // Use this to make a bit where I get all the geolocated stuff and write to file.
    $size = 'small';
    if (array_key_exists('size', $_GET)) {
      $size = $_GET['size'];
    }

    if (array_key_exists('generate', $_GET)) {
      $gen = $_GET['generate'];
    }

    $cwd = getcwd();
    $path = drupal_get_path('module', 'islandora_map_browse');
    $dataPath = $cwd . "/" . $path . "/data/AllGEOLocated.json";

    // Check to see if the pregenerated file exists
    /*
    if(file_exists($dataPath) == 1){

    $response_data = json_decode(file_get_contents($dataPath), true);

    $this->prepareGEOJSONVariablesConfig($solrQueryProcessor);
    $out = $this->prepareSolrResultsGEOJSONConfig($response_data);
    watchdog("islandora_map_browse", "using file");
    }else{

    watchdog("islandora_map_browse", "generating new AllGEOLocated file");
     */
    $increment = 13000;
    $lowerlimit = ((($page + 1) * $increment) - $increment);
    // drupal_set_message(var_dump($_GET));
    // Set up the variables that get passed to javascript.
    $this->prepareGEOJSONVariablesConfig($solrQueryProcessor);

    // $queryInfoForJS = array();
    $response_data = $solrQueryProcessor->islandoraSolrResult['response'];

    // Dump the following into drupal set message to see what your dealing with var_dump to debug.
    $res = $solrQueryProcessor;

    // The fields added here are required.
    $fl_params = "PID, RELS_EXT_hasModel_uri_ms, " . variable_get('map_browse_return_fields') . ", " . variable_get('map_browse_title_field') . ", " . variable_get('map_browse_coord_field');
    // RELS_EXT_hasModel_uri_ms, mods_location_url_ms,
    // var_dump($fl_params);
    $solrQueryProcessor->solrParams['fl'] = array($fl_params);

    // This section resets the limits on the query and re-executes.
    $upperlimit = $solrQueryProcessor->islandoraSolrResult['response']['numFound'];

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

  }//End of mapPageResults


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

    $cluster_path = libraries_get_path('Leaflet.markercluster-master');
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

    /***************************************************************/
    /*****Parse for leaflet *******************/
    $leafletData = array();
    foreach ($response_data['objects'] as $object_result) {
      $doc = $object_result['solr_doc'];
      $thumb = $object_result['thumbnail_url'];
      $obj = $object_result['object_url'];
      //$coordinatesLatLon = NULL;

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
          $displayFields = explode(",", variable_get('map_browse_return_fields'));
          //$tdisplayFields = array();

          // Check to see if the record 'doc' has any of the fields configured for display
          // For now hard code in the proper field names.  Maybe get this a configuration option or grab from
          // the solr module.
          $displayNames = array("dc.contributor" => "Contributor", "mods_location_physicalLocation_ms" => "Physical Location", "dc.publisher" => "Publisher");
          $confFields = '';
          $uniqueItems = $uniqueItems + 1;
          //foreach ($displayFields as $k => $v) {
          foreach ($displayFields as $v) {
            $tv = trim($v);
            //$tdisplayFields[$tv] = $tv;
            if (array_key_exists($tv, $doc) && $doc[$tv][0] != '') {
              //$props[$tv] = $doc[$tv];
              if (strcmp($confFields, '') == 0) {
                $confFields = '<p>' . $displayNames[$tv] . ': ' . $doc[$tv][0] . '</p>';
              }
              else {
                $confFields = '<p>' . $displayNames[$tv] . ': ' . $doc[$tv][0] . '</p>' . $confFields;
              }
            }
          }

          // watchdog("map view", $doc['dc.title']);.
          $title = $doc['dc.title'][0];
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

    // $pinInfo['pinsCount'] = $withVal;
    // $pinInfo['nopinsCount'] = $withoutVal;.
    $pinInfo['uniqueItems'] = $uniqueItems;
    $pinInfo['uniqueLocations'] = count($leafletData);

    if (strpos($_SERVER[‘HTTP_ACCEPT_ENCODING’], ‘gzip’) !== FALSE) {
      ob_start(“ob_gzhandler”);
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

}//End of class

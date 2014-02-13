<?php
/*
error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('display_startup_errors', TRUE);
*/


module_load_include('inc', 'islandora_solr_search', 'IslandoraSolrResults');

/**
 * Extention of IslandoraSolrResults for templating purposes.
 * This overrides the displayResults function to provide an alternate display type.
 */

class IslandoraSolrResultsMapView extends IslandoraSolrResults {

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
function mapResults($solrQueryProcessor) {

	 $withVal = 0;
	 $withoutVal = 0;

	 $response_data = $solrQueryProcessor->islandoraSolrResult['response'];
	 //Dump the following into drupal set message to see what your dealing with
	 $res = $solrQueryProcessor;
	 
	 //Set up the variables that get passed to javascript
	 $this->prepareGEOJSONVariables($solrQueryProcessor);

	 //This section resets the limits on the query and re-executes
	 $upperlimit = $solrQueryProcessor->islandoraSolrResult['response']['numFound'];
	 $lowerlimit = 0;
	 $increment = 5000;
	 //variable_get('map_browse_max_row_to_render');
	 $solrQueryProcessor->solrStart = $lowerlimit;
	 $solrQueryProcessor->solrLimit = $increment;

	 $solrQueryProcessor->resetResults();
	 $solrQueryProcessor->executeQuery(FALSE);
	 $response_data = $solrQueryProcessor->islandoraSolrResult['response'];
	 $out = $this->prepareSolrResultsGEOJSON($response_data);


return $out;

}//End of mapResult


  /**
   * @param $results
   *   A solr resultset object.
   *
   * @return
   *   html output for the resultset.
   */
function mapPageResults($solrQueryProcessor) {

	 $page = 0;
	 $url_args = array();
	 $url_args = parse_str(parse_url($_SERVER['HTTP_REFERER'], PHP_URL_QUERY));
	 $increment = 20;
	 $lowerlimit = ((($page + 1)*$increment)-$increment);

	 //Set up the variables that get passed to javascript
	 $this->prepareGEOJSONVariables($solrQueryProcessor);

	 //$queryInfoForJS = array();
	 $response_data = $solrQueryProcessor->islandoraSolrResult['response'];

	 //Dump the following into drupal set message to see what your dealing with var_dump to debug
	 $res = $solrQueryProcessor;

	 //This section resets the limits on the query and re-executes
	 $upperlimit = $solrQueryProcessor->islandoraSolrResult['response']['numFound'];

	 $solrQueryProcessor->solrStart = $lowerlimit;
	 $solrQueryProcessor->solrLimit = $increment;

	 $solrQueryProcessor->resetResults();
	 $solrQueryProcessor->executeQuery(FALSE);
	 $response_data = $solrQueryProcessor->islandoraSolrResult['response'];
	 $out = $this->prepareSolrResultsGEOJSON($response_data);

return $out;

}//End of mapPageResults





//*********** GEOJSON CODE ***********************************

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
	 $increment = 20;
	 $lowerlimit = ((($page + 1)*$increment)-$increment);

	 //Set up the variables that get passed to javascript
	 $this->prepareGEOJSONVariablesConfig($solrQueryProcessor);

	 //$queryInfoForJS = array();
	 $response_data = $solrQueryProcessor->islandoraSolrResult['response'];

	 //Dump the following into drupal set message to see what your dealing with var_dump to debug
	 $res = $solrQueryProcessor;

	 //The fields added here are required
	 $fl_params = "PID, RELS_EXT_hasModel_uri_ms, " . variable_get('map_browse_return_fields') . ", ". variable_get('map_browse_title_field') . ", ". variable_get('map_browse_coord_field');	 
	 //RELS_EXT_hasModel_uri_ms, mods_location_url_ms,

	 //var_dump($fl_params);
	 $solrQueryProcessor->solrParams['fl'] = array($fl_params);
	 
	 	 
	 //This section resets the limits on the query and re-executes
	 $upperlimit = $solrQueryProcessor->islandoraSolrResult['response']['numFound'];

	 $solrQueryProcessor->solrStart = $lowerlimit;
	 $solrQueryProcessor->solrLimit = $increment;

	 $solrQueryProcessor->resetResults();
	 $solrQueryProcessor->executeQuery(FALSE);
	 $response_data = $solrQueryProcessor->islandoraSolrResult['response'];
//var_dump($response_data);
	 $out = $this->prepareSolrResultsGEOJSONConfig($response_data);

return $out;

}//End of mapPageResults


/**
 * @param $solrQueryProcessor
 * A solr query object
 *
 * @return nothing
 * Passes information needed by javascript to process results 
 */
function prepareGEOJSONVariables($solrQueryProcessor){

	 drupal_add_js(drupal_get_path('module', 'islandora_map_browse') . '/js/open_map_GEOJSON_location.js', 'file');
	 drupal_add_js(drupal_get_path('module', 'islandora_map_browse') . '/js/OpenLayers-2.12/OpenLayers.js', 'file');
	 $InfoForJS['qt'] = $solrQueryProcessor->solrQuery;
	 $InfoForJS['numFound'] = $solrQueryProcessor->islandoraSolrResult['response']['numFound'];
	 $InfoForJS['rows'] = $solrQueryProcessor->solrLimit;
	 $InfoForJS['baseUrl'] = variable_get('map_browse_fedora_base');
	 $InfoForJS['fedoraPrefix'] = variable_get('map_browse_fedora_prefix');
	 $InfoForJS['fedoraSuffix'] = variable_get('map_browse_fedora_suffix');
	 $jsInfo = json_encode($InfoForJS);
	 drupal_add_js(array('islandora_map_browse_settings_jsInfo' => $jsInfo), 'setting');
}


/**
 * @param $solrQueryProcessor
 * A solr query object
 *
 * @return nothing
 * Passes information needed by javascript to process results 
 */
function prepareGEOJSONVariablesConfig($solrQueryProcessor){

	 drupal_add_js(drupal_get_path('module', 'islandora_map_browse') . '/js/open_map_GEOJSON_location_config.js', 'file');
	 drupal_add_js(drupal_get_path('module', 'islandora_map_browse') . '/js/OpenLayers-2.12/OpenLayers.js', 'file');
	 $InfoForJS['qt'] = $solrQueryProcessor->solrQuery;
	 $InfoForJS['numFound'] = $solrQueryProcessor->islandoraSolrResult['response']['numFound'];
	 $InfoForJS['rows'] = $solrQueryProcessor->solrLimit;
	 $InfoForJS['baseUrl'] = variable_get('map_browse_fedora_base');
	 $InfoForJS['fedoraPrefix'] = variable_get('map_browse_fedora_prefix');
	 $InfoForJS['fedoraSuffix'] = variable_get('map_browse_fedora_suffix');
	 $jsInfo = json_encode($InfoForJS);
	 drupal_add_js(array('islandora_map_browse_settings_jsInfo' => $jsInfo), 'setting');
}




/**
 * @params $solrResultSet
 *
 * @return HTML output
 *
 * This functions dedups the results of the query and send them to javascript
 * it also passes some information about the result sets
 *
 */ 
function prepareSolrResultsGEOJSON ($response_data) {
$withVal = 0;
$withoutVal = 0;

//var_dump($response_data);

$mods_location = variable_get('map_browse_coord_field');
$map_centre = variable_get('map_browse_map_centre');

$object_results = $response_data['objects'];
//$newpins = array();
$nopins = array();         
$geoArr = array("type" => "FeatureCollection", "features" => array());
$featuresArr = array();
foreach ($response_data['objects'] as $object_result) {
	$doc = $object_result['solr_doc'];
	$thumb = $object_result['thumbnail_url'];
	$obj = $object_result['object_url'];
	$coordinatesLatLon = null;		
	//Need to put a check in here to avoid the undefined index error
	if( isset($doc[$mods_location]) && $doc[$mods_location] != ''){
              $withVal++;
	      $parts = explode(",",$doc[$mods_location][0]);
	      $lat = floatval($parts[0]);
	      $lon = floatval($parts[1]);

	      if( isset($doc["mods_location_physicalLocation_ms"])){
	      	  $geoArr["features"][] = array("type" => "Feature", "geometry" => array("type" => "Point", "coordinates" => array($lon,$lat)), "properties" => array ("title" => $doc["dc.title"], "contrib" => $doc["dc.contributor"], "loc" => $doc["mods_location_physicalLocation_ms"],"desc" => $doc["dc.description"],"obj_url" => $obj,"thumb_url" => $thumb)); 
	      }else{
		  $geoArr["features"][] = array("type" => "Feature", "geometry" => array("type" => "Point", "coordinates" => array($lon,$lat)), "properties" => array ("title" => $doc["dc.title"], "contrib" => $doc["dc.contributor"],"desc" => $doc["dc.description"],"obj_url" => $obj,"thumb_url" => $thumb)); 
		  }
	}else{
		if(strcmp($doc["RELS_EXT_hasModel_uri_ms"]['0'], "info:fedora/islandora:collectionCModel") != 0 ){
			$nopins[$doc['PID']] = array ("obj_url" => $obj,"thumb_url" => $thumb);
			if(isset($doc["dc.title"])){
				$nopins[$doc['PID']]["title"] = $doc["dc.title"]; 
			}			
			if(isset($doc["dc.contributor"])){
				$nopins[$doc['PID']]["contrib"] = $doc["dc.contributor"]; 
			}			
			if(isset($doc["dc.description"])){
				$nopins[$doc['PID']]["desc"] = $doc["dc.description"]; 
			}			
			$withoutVal++; 
	  	}
       	  }
}

//print_r($geoArr);

$pinInfo['pinsCount'] = $withVal;
$pinInfo['nopinsCount'] = $withoutVal;

//Encode everything that goes to JS
$jsPinInfo = json_encode($pinInfo);
$jnopins = json_encode($nopins);
$geopins = json_encode($geoArr);

drupal_add_js(array('islandora_map_browse_settings' => array('centre' => $map_centre)), 'setting');
drupal_add_js(array('islandora_map_browse_settings_pininfo' => $jsPinInfo), 'setting');
drupal_add_js(array('islandora_map_browse_settings_nopins' => $jnopins), 'setting');
drupal_add_js(array('islandora_map_browse_settings_geopins' => $geopins), 'setting');


if($withoutVal == 0){
	       $output = '<div id="info_canvas" style="width:800px; height:60px;"></div><div id="map_canvas" style="width:800px; height:800px; float: left;"></div><div id="text_canvas" style="width:30px; height:600px; float: right; overflow: auto;"></div>';
}else{
    $output = '<div id="info_canvas" style="width:600px; height:60px;"></div><div id="map_canvas" style="width:600px; height:600px; float: left;"></div><div id="text_canvas" style="width:300px; height:600px; float: right; overflow: auto;"></div>';

}
   return $output;

}






/**
 * @params $solrResultSet
 *
 * @return HTML output
 *
 * This functions dedups the results of the query and send them to javascript
 * it also passes some information about the result sets
 *
 */ 
function prepareSolrResultsGEOJSONConfig ($response_data) {
$withVal = 0;
$withoutVal = 0;

//var_dump($response_data);

$mods_location = variable_get('map_browse_coord_field');
$map_centre = variable_get('map_browse_map_centre');

$object_results = $response_data['objects'];
//$newpins = array();
$nopins = array();         
$geoArr = array("type" => "FeatureCollection", "features" => array());
$newgeo = array("type" => "FeatureCollection", "features" => array());
$featuresArr = array();
foreach ($response_data['objects'] as $object_result) {
	$doc = $object_result['solr_doc'];
	$thumb = $object_result['thumbnail_url'];
	$obj = $object_result['object_url'];
	$coordinatesLatLon = null;		
	//Need to put a check in here to avoid the undefined index error
	if( isset($doc[$mods_location]) && $doc[$mods_location] != ''){
              $withVal++;
	      $parts = explode(",",$doc[$mods_location][0]);
	      $lat = floatval($parts[0]);
	      $lon = floatval($parts[1]);
	      $props = array("title" => $doc['dc.title'], "obj_url" => $obj, "thumb_url" => $thumb);      	      
		      
	      $displayFields = explode(",", variable_get('map_browse_return_fields'));
	      $tdisplayFields = array();
	      foreach($displayFields as $k => $v){
	      			     $tv = trim($v);
				     $tdisplayFields[$tv] = $tv;
				     if(array_key_exists($tv, $doc)){
				     $props[$tv] = $doc[$tv];
				     }
				    }
	      $newgeo["features"][] = array("type" => "Feature", "geometry" => array("type" => "Point", "coordinates" => array($lon,$lat)), "properties" => $props);

	}else{
		if(strcmp($doc["RELS_EXT_hasModel_uri_ms"]['0'], "info:fedora/islandora:collectionCModel") != 0 ){
			$nopins[$doc['PID']] = array ("obj_url" => $obj,"thumb_url" => $thumb);
			if(isset($doc["dc.title"])){
				$nopins[$doc['PID']]["title"] = $doc["dc.title"]; 
			}			
			if(isset($doc["dc.contributor"])){
				$nopins[$doc['PID']]["contrib"] = $doc["dc.contributor"]; 
			}			
			if(isset($doc["dc.description"])){
				$nopins[$doc['PID']]["desc"] = $doc["dc.description"]; 
			}			
			$withoutVal++; 
	  	}
       	  }
}

//print_r($geoArr);

$pinInfo['pinsCount'] = $withVal;
$pinInfo['nopinsCount'] = $withoutVal;

//Encode everything that goes to JS
$displayfieldsJS = json_encode($tdisplayFields);
$jsPinInfo = json_encode($pinInfo);
$jnopins = json_encode($nopins);
$geopins = json_encode($newgeo);
//var_dump($displayfieldsJS);
drupal_add_js(array('islandora_map_browse_settings' => array('centre' => $map_centre)), 'setting');
drupal_add_js(array('islandora_map_browse_settings_pininfo' => $jsPinInfo), 'setting');
drupal_add_js(array('islandora_map_browse_settings_nopins' => $jnopins), 'setting');
drupal_add_js(array('islandora_map_browse_settings_geopins' => $geopins), 'setting');
drupal_add_js(array('islandora_map_browse_settings_displayfields' => $displayfieldsJS), 'setting');


if($withoutVal == 0){
	       $output = '<div id="info_canvas" style="width:800px; height:60px;"></div><div id="map_canvas" style="width:800px; height:800px; float: left;"></div><div id="text_canvas" style="width:30px; height:600px; float: right; overflow: auto;"></div>';
}else{
    $output = '<div id="info_canvas" style="width:600px; height:60px;"></div><div id="map_canvas" style="width:600px; height:600px; float: left;"></div><div id="text_canvas" style="width:300px; height:600px; float: right; overflow: auto;"></div>';

}
   return $output;

}












}


<?php

  /*Grab the file being parsed with the following command line snippet*/
  /*curl "http://sho-islandora-live.usask.ca:8080/solr/select?indent=on&version=2.2&q=PID%3Amilne*+and+dc.date%3A%5B*+TO+*%5D&fq=&start=0&rows=15000&fl=*&wt=json&explainOther=&hl.fl="*/

  $str = file_get_contents('out');
$response_data = json_decode($str, true); // decode the JSON into an associative array

//  echo '<pre>' . print_r($json, true) . '</pre>';



$newgeo = array("type" => "FeatureCollection", "features" => array());


foreach ($response_data['response']['docs'] as $k => $doc){
  $thumbnail_url = "http://sho.usask.ca/islandora/object/".$doc['PID']. "/datastream/TN/view";
  $object_url = "http://sho.usask.ca/islandora/object/". $doc['PID'];
	if( isset($doc[$mods_location]) && $doc[$mods_location] != ''){
              $withVal++;
	      $parts = explode(",",$doc[$mods_location][0]);
	      //Check to see if the location is in the correct format.  If not report to error log
	      if ( !isset($parts[1]) || !isset($parts[1])) {
	      	 //var_dump($doc['PID']);
		 //http://sho-islandora-live.usask.ca/islandora/object/islandora%3A32650
		 //variable_get('map_browse_fedora_prefix');
		 //variable_get('map_browse_fedora_base');
		 watchdog("mods_location bad format", "See: " .$doc['PID']);
	      }else{
 
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


var_dump($newgeo);



break;


foreach ($response_data['objects'] as $object_result) {
	$doc = $object_result['solr_doc'];
	$thumb = $object_result['thumbnail_url'];
	$obj = $object_result['object_url'];
	$coordinatesLatLon = null;		
	//Need to put a check in here to avoid the undefined index error
	if( isset($doc[$mods_location]) && $doc[$mods_location] != ''){
              $withVal++;
	      $parts = explode(",",$doc[$mods_location][0]);
	      //Check to see if the location is in the correct format.  If not report to error log
	      if ( !isset($parts[1]) || !isset($parts[1])) {
	      	 //var_dump($doc['PID']);
		 //http://sho-islandora-live.usask.ca/islandora/object/islandora%3A32650
		 //variable_get('map_browse_fedora_prefix');
		 //variable_get('map_browse_fedora_base');
		 watchdog("mods_location bad format", "See: " .$doc['PID']);
	      }else{
 
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




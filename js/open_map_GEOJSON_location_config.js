/**
 * @file
 */
Drupal.behaviors.islandora_map_browse = {
    attach: function (context) {
      var map;
      initialize();
    }
};



function initialize() {

  // Here is where the php objects are passed to the javascript
  // var centre = Drupal.settings.islandora_map_browse_settings.centre;.
  var lat = Drupal.settings.islandora_map_browse_settings_centreLat;
  var lon = Drupal.settings.islandora_map_browse_settings_centreLon;
  var zoom = Drupal.settings.islandora_map_browse_settings_zoom;

  var pininfo = Drupal.settings.islandora_map_browse_settings_pininfo;
  // Var geopins = Drupal.settings.islandora_map_browse_settings_geopins;.
  var leafPins = Drupal.settings.islandora_map_browse_settings_leafletpins;
  var jsInfo = Drupal.settings.islandora_map_browse_settings_jsInfo;
  var raw = Drupal.settings.islandora_map_browse_settings_raw;
  var result = '';
  // Var displayfields = Drupal.settings.islandora_map_browse_settings_displayfields;
  // prepare the data.
  var pinInfo = JSON.parse(pininfo);
  var confObj = JSON.parse(jsInfo);
  var searchTerm = confObj.qt;
  var numFound = confObj.numFound;
  var rows = confObj.rows;
  // Var baseUrl = confObj.baseUrl;
  //    var fedoraPrefix = confObj.fedoraPrefix;
  //    var fedoraSuffix = confObj.fedoraSuffix;
  // var objectPrefix = confObj.objectPrefix;
  // var jsLib = confObj.js_library;.
  var withLocationCnt = pinInfo.pinsCount;
  var uniqueLocations = pinInfo.uniqueLocations;
  var uniqueItems = pinInfo.uniqueItems;

  // console.log(jsLib);
  var locationResults = 0;
  var topBar = document.getElementById('info_canvas');
  var matched = new Boolean();

  //
  // This builds up the top bar content
  //
  // topBar.innerHTML = "There where " + withLocationCnt + " records with location information of a possible " + numFound;.
  topBar.innerHTML = "There are " + uniqueLocations + " unique locations with " + uniqueItems + " unique items";
  // console.log(leafPins);
  // Leaflet JS code
  // if(jsLib == 2){
  // Create map.
  map = L.map('map_canvas', {
    // console.log("centre: " + lat + lon);.
	  layers:MQ.mapLayer(),
    center: [lat, lon],
        minZoom: 2,
        zoom: zoom
  });

  //L.tileLayer('http://{s}.mqcdn.com/tiles/1.0.0/map/{z}/{x}/{y}.png', {
  //      attribution: '&copy; <a href="http://osm.org/copyright" title="OpenStreetMap" target="_blank">OpenStreetMap</a> contributors | Tiles Courtesy of <a href="http://www.mapquest.com/" title="MapQuest" target="_blank">MapQuest</a> <img src="http://developer.mapquest.com/content/osm/mq_logo.png" width="16" height="16">',
  //subdomains: ['otile1','otile2','otile3','otile4']
  //        }).addTo(map);

  var markers = L.markerClusterGroup();

  for (var key in leafPins) {
    if (leafPins.hasOwnProperty(key)) {
      var marker = L.marker([leafPins[key].lat, leafPins[key].lon]);
      marker.bindPopup(leafPins[key].content, {maxHeight:250, minWidth:250});
      markers.addLayer(marker);
    }
  }
  map.addLayer(markers);
  // }.
}

Drupal.behaviors.islandora_map_browse = {
    attach: function (context) {
    var map;    
    initialize();
    }
};



function initialize() {

    //Here is where the php objects are passed to the javascript
    //var centre = '52.1311, -106.6353';
    centre = Drupal.settings.islandora_map_browse_settings.centre;
    var centreLocation = getPosition(centre);
    var pininfo = Drupal.settings.islandora_map_browse_settings_pininfo;
    var pins = Drupal.settings.islandora_map_browse_settings_pins;
    var nopins = Drupal.settings.islandora_map_browse_settings_nopins;
    var jsInfo = Drupal.settings.islandora_map_browse_settings_jsInfo;
    var raw = Drupal.settings.islandora_map_browse_settings_raw;
    var result = '';

    //prepare the data
    var pinInfo = JSON.parse(pininfo);
    var confObj = JSON.parse(jsInfo);
    var searchTerm = confObj.qt;
    var numFound = confObj.numFound;
    var rows = confObj.rows;
    var baseUrl = confObj.baseUrl;
    var fedoraPrefix = confObj.fedoraPrefix;
    var fedoraSuffix = confObj.fedoraSuffix;
    //var objectPrefix = confObj.objectPrefix;
    var withLocationCnt = pinInfo.pinsCount;
    var withNoLocationCnt = pinInfo.nopinsCount;
    //Other vars
    var pinsObject = JSON.parse(pins);
    var markers = [];
    var noPinsObj = JSON.parse(nopins);
    var locationResults = 0;
    var sideBar = document.getElementById('text_canvas');
    var topBar = document.getElementById('info_canvas');
    var matched = new Boolean();

    //A bit of debug code
    var dumpObject = JSON.stringify(noPinsObj);
    //console.log(dumpObject);

    /////////////////////////////////////////////
    //This is the right hands side text area code
    /////////////////////////////////////////////
    //console.log("count of no locaton:" +withNoLocationCnt);
    if(withNoLocationCnt > 0){
	sideBar.innerHTML = sidebarContent(noPinsObj);
    }


    ////////////////////////////////////
    //This builds up the top bar content
    ////////////////////////////////////
    topBar.innerHTML = "There where " + withLocationCnt + " records with location information of a possible " + numFound;

    /////////////////////////////
    //Below is the map area code
    /////////////////////////////
    //Add controls to the map
    var options = {
	controls: [
		   new OpenLayers.Control.Navigation(),
		   new OpenLayers.Control.PanZoomBar(),
		   new OpenLayers.Control.Attribution()
          ]
    };

    //Create a base layer for the map
    map = new OpenLayers.Map("map_canvas", options);
    var mapLayer         = new OpenLayers.Layer.OSM();
    var fromProjection = new OpenLayers.Projection("EPSG:4326");   // Transform from WGS 1984
    var toProjection   = new OpenLayers.Projection("EPSG:900913"); // to Spherical Mercator Projection
    var position       = new OpenLayers.LonLat(-106.6353, 52.1311).transform( fromProjection, toProjection);
    var zoom           = 8; 
 
    map.addLayer(mapLayer);
    map.setCenter(position, zoom );

    
    //Set up a style for the points.  May need to style multi image points differently for clarity
    //the point size can represent either number of images below a point or the number of 
    //points in an area.
    var myStyles = new OpenLayers.StyleMap({
	    "default": new OpenLayers.Style({
                    pointRadius: "${s}", // sized according to type attribute
                    fillColor: "#ff0000",
                    strokeColor: "#000000",
                    strokeWidth: 2,
                    graphicZIndex: 1
                }),
	    "select": new OpenLayers.Style({
                    fillColor: "#0000ff",
                    strokeColor: "#3399ff",
                    graphicZIndex: 2
                })
	}
	
	);



    //Create and array of point object adding the metadata to an attribute named content 
    var pointsArr = [];
    var pointID = 0;
    //Loop through the JSON object
    for( var latLong in pinsObject){
	var p = getPosition(latLong);
	var windowContent = '';
        var pinCnt = 0;
	//for each location go throught he subobject and grab the metadata
	for( var pid in pinsObject[latLong] ){
	    pinCnt = pinCnt + 1;
	    var title = pinsObject[latLong][pid].title;
	    var contrib = pinsObject[latLong][pid].contrib;
	    var loc = pinsObject[latLong][pid].loc;
	    var desc = pinsObject[latLong][pid].desc;
	    //Add the metadata to a chunk of html for display in the popup
	    windowContent = windowContent + newbuildInfoWindow(pid, title, contrib, loc, desc);
	}
	//Add points to array with metadata as content attribute
	if(pinCnt > 5){
	    pinCnt = 5;
	}
	pointsArr[pointID] = new OpenLayers.Feature.Vector(p,{content:windowContent, s: (pinCnt * 2) + 3});
	pointID = pointID + 1;
    }

    var strategy = new OpenLayers.Strategy.Cluster({distance: 50});

    // create the layer with listeners to create and destroy popups
    var vector = new OpenLayers.Layer.Vector("Points",{
	    eventListeners:{
		'featureselected':function(evt){
		    var feature = evt.feature;
		    var popup = new OpenLayers.Popup.FramedCloud("popup",
								 OpenLayers.LonLat.fromString(feature.geometry.toShortString()),
								 null,
								 feature.attributes.content,
								 null,
								 true
								 );
		    feature.popup = popup;
		    map.addPopup(popup);
		},
		'featureunselected':function(evt){
		    var feature = evt.feature;
		    map.removePopup(feature.popup);
		    feature.popup.destroy();
		    feature.popup = null;
		}
	    },
	    styleMap:myStyles,
	    strategies: [new OpenLayers.Strategy.Cluster()],
	});

    vector.addFeatures(pointsArr);

    // create the select feature control
    var selector = new OpenLayers.Control.SelectFeature(vector,{
	    hover:false,
	    autoActivate:true
	}); 

    map.addLayer(vector);
    map.addControl(selector);

    /////////////////////////////////////
    //Below here are the helper functions
    /////////////////////////////////////
 
    //This takes the lat/long information from the solr query and outputs a Point object
    //Notice that the object uses long/lat
    function getPosition(loc){
	//console.log(loc);
        var commaPos = loc.indexOf(',');
        var coordinatesLat = parseFloat(loc.substring(0, commaPos));
        var coordinatesLong = parseFloat(loc.substring(commaPos + 1, loc.length));
	var myLatlng = new OpenLayers.Geometry.Point(coordinatesLong, coordinatesLat).transform( fromProjection, toProjection);
        return myLatlng;
    }

    //This builds the content of the popups
    function newbuildInfoWindow(p, title, contrib, loc, desc){

    infoString = "<div style='border-bottom:1px solid black'>" +
        "<h2><a href='"+baseUrl+fedoraPrefix+pid+"'>"+ title +"</a></h2>"+
        "<img width='254' height='355' src='"+baseUrl+fedoraPrefix+p+fedoraSuffix+"'>" +
        "<h4>"+ contrib +"</h4>"+
            "<h4>"+ loc +"</h4>"+
        "</div>";

    return infoString
        }

    //This function builds up the right sidebar content
    function sidebarContent(doc){
	var infoString;
	for( var pid in doc ){
	    if(infoString){
         infoString = infoString + "<div id=\"side-bar-box\" style=\"border-top-style:solid;border-width:1px\"><a href='"+baseUrl+fedoraPrefix+pid+"'>"+ doc[pid].title+"</a>"+
        "<div id=\"overlay_form_"+pid+"\" style=\"display:block;margin-left:auto;margin-right:auto\">"+
        "<img src='"+baseUrl+fedoraPrefix+pid+fedoraSuffix+"'>" +
	     "</br>"+
	     "</div>";
	    }else{
            infoString = "<a href='"+baseUrl+fedoraPrefix+pid+"'>"+ doc[pid].title+"</a>"+
        "<div id=\"overlay_form_"+pid+"\" style=\"display:inline\">"+

        "<img src='"+baseUrl+fedoraPrefix+pid+fedoraSuffix+"'>" +
		"</br>"+
		"</div></div>";
	    }
	}
        return infoString

            }




}


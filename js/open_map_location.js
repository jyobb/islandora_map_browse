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
    var objectPrefix = confObj.objectPrefix;
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
                    pointRadius: "${radius}", // sized according to type attribute
		    label: "${label}",
		    fillColor: "#ffbb44",
                    strokeColor: "#ffffff",
                    strokeWidth: 1,
                    graphicZIndex: 1
                },
    {
	context: {
	    radius: function(feature) {
		var pix = 12;
		//console.log(feature.cluster);
		//if(feature.cluster){
		    //pix = Math.min(feature.attributes.count, 7) + 3;
		//}
		return pix;
	    },
	    label:function(feature) {
		var lab = 1;
		//console.log(JSON.stringify(feature.cluster));
		//console.log(feature.attributes.count);
		for(var key in feature.cluster){

		    //console.log(JSON.stringify(feature.cluster[key].data));

		}
		//console.log(feature['data'].toString());
		//here we can drill down into the feature data and see how may items are below
		//This can be compared with the attribute count and when the attribute count gets
		//to 1 we switch color to something else.
		if(feature.cluster){
		    lab = feature.attributes.count;
		}
		return lab;
	    }
	}
    }
		),
	    "select": new OpenLayers.Style({
                    fillColor: "#0000ff",
                    strokeColor: "#3399ff",
                    graphicZIndex: 2
                }),
	});



    //Create and array of point object adding the metadata to an attribute named content 
    var pointsArr = [];
    var pointID = 0;
    var feature, features = [];
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
	//pointsArr[pointID] 
	feature = new OpenLayers.Feature.Vector(p,{content:windowContent, s: (pinCnt * 2) + 3});
	features.push(feature);
	//pointID = pointID + 1;
    }

    var strategy = new OpenLayers.Strategy.Cluster({distance: 100});

    // create the layer with listeners to create and destroy popups
    var vector = new OpenLayers.Layer.Vector("Points",{
	    strategies: [strategy],
	    styleMap:myStyles,
	    
	    eventListeners:{
		'featureselected':function(evt){
		    var feature = evt.feature;
		    var popupContent = '';
		    //look into adding id to each cluster and checking this before clicking
		    for(var key1 in feature.cluster){
			console.log(":::::::::" + JSON.stringify(evt.feature.cluster[key1].data.content));
		    }
		    for(var key in feature.cluster){
			popupContent = popupContent + evt.feature.cluster[key].data.content
		    //console.log(JSON.stringify(feature.cluster[key].data));

		}
		    

		    var popup = new OpenLayers.Popup.FramedCloud("popup",
								 OpenLayers.LonLat.fromString(feature.geometry.toShortString()),
								 null,
								 popupContent,
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
	});
    map.addLayer(vector);
    vector.addFeatures(features);

    // create the select feature control
    var selector = new OpenLayers.Control.SelectFeature(vector,{
	    hover:false,
	    autoActivate:true
	}); 


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
        "<img width='220' height='355' src='"+baseUrl+fedoraPrefix+p+fedoraSuffix+"'>" +
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
            infoString = "<a href='"+baseUrl+objectPrefix+pid+"'>"+ doc[pid].title+"</a>"+
        "<div id=\"overlay_form_"+pid+"\" style=\"display:inline\">"+

        "<img src='"+baseUrl+fedoraPrefix+pid+fedoraSuffix+"'>" +
		"</br>"+
		"</div></div>";
	    }
	}
        return infoString

            }




}


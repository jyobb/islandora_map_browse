Drupal.behaviors.islandora_map_browse = {
    attach: function (context) {
    var map;    
    initialize();
    }
};



function initialize() {

    //Here is where the variables are passed to the javascript
    var centre = '52.1311, -106.6353';
    centre = Drupal.settings.islandora_map_browse_settings.centre;
    var centreLocation = getPosition(centre);
    var pins = Drupal.settings.islandora_map_browse_settings_pins;
    var nopins = Drupal.settings.islandora_map_browse_settings_nopins;
    var jsInfo = Drupal.settings.islandora_map_browse_settings_jsInfo;
    var raw = Drupal.settings.islandora_map_browse_settings_raw;

    var mapOptions = {
	//center: new google.maps.LatLng(coordinatesLat, coordinatesLong),
	center: centreLocation,
	zoom: 8,
	mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    map = new google.maps.Map(document.getElementById("map_canvas"),
				  mapOptions);
        
    //Information about the search and system
    var confObj = JSON.parse(jsInfo);
    var searchTerm = confObj.qt;
    var numFound = confObj.numFound;
    var rows = confObj.rows;
    var baseUrl = confObj.baseUrl;
    var fedoraPrefix = confObj.fedoraPrefix;
    var fedoraSuffix = confObj.fedoraSuffix;
    var objectPrefix = confObj.objectPrefix;
    var withLocationCnt = confObj.pinsCount;
    var withNoLocationCnt = confObj.nopinsCount;
    //Other vars
    var myObject = JSON.parse(pins);
    var markers = [];
    var noPinsObj = JSON.parse(nopins);
    var locationResults = 0;
    var sideBar = document.getElementById('text_canvas');
    var topBar = document.getElementById('info_canvas');
    var matched = new Boolean();

    for( var latLong in myObject){
	//console.log(latLong);
	    var p = getPosition(latLong);
	    var windowContent;
	    for( var pid in myObject[latLong] ){
		var title = myObject[latLong][pid].title;
		var contrib = myObject[latLong][pid].contrib;
		var loc = myObject[latLong][pid].loc;
		var desc = myObject[latLong][pid].desc;
		windowContent = windowContent + newbuildInfoWindow(pid, title, contrib, loc, desc);
		//console.log(myObject[latLong][pid].title);
		//console.log(pid);
	    }
	    var markerStuff = {"p":p, "content": windowContent, "cnt":1};
	    markers.push(markerStuff);
    }

    //Handle the non pins and page query info
    sideBar.innerHTML = sidebarContent(noPinsObj);
    topBar.innerHTML = "There where " + withLocationCnt + " records with location information of a possible " + numFound;

    //Now push the markers onto the map 
    locationResults = markers.length;
    //    console.log('loc res: ' +locationResults);

    var cluster = new MarkerClusterer(map, {gridSize:80, maxZoom:14, minimumClusterSize:4,});
    var infowindow = new google.maps.InfoWindow({
	    size: new google.maps.Size(150,150)
	});



    markers.forEach( function(elem){
	    	    placeMarker(elem.p, elem.content, elem.cnt);	    
	    /*
	var marker = new google.maps.Marker({
		position: elem.p,
	    map: map,
	    title: elem.cnt.toString() + ' items at this location',
	    });

	google.maps.event.addListener(marker, 'click', function () {
	    infowindow.setContent(elem.content.info);
	    infowindow.open(map, marker);
	});
	cluster.addMarker(marker);
	    */
	});

    //Functions below here



    function placeMarker(location, info, cnt) {

	var marker = new google.maps.Marker({
	    position: location,
	    map: map,
	    title: cnt.toString() + ' items at this location',
	    });

	google.maps.event.addListener(marker, 'click', function () {
	    infowindow.setContent(info);
	    infowindow.open(map, marker);
	});
	cluster.addMarker(marker);
    }





function sidebarContent(doc){
    var infoString;
    for( var pid in doc ){
	if(infoString){ 
         infoString = infoString + "<a href='"+baseUrl+objectPrefix+pid+"'>"+ doc[pid].title+"</a>"+
	     //"<p>"+ doc[pid].contrib +"</p>"+
	     //"<p>"+ doc[pid].loc +"</p>" +
	     //"<div class=\"popper\"><a href=\"#\" id=\""+pid+"\">PopUp</a></div>"+
	"<div id=\"overlay_form_"+pid+"\" style=\"display:inline\">"+
	     //"<h2>" + doc[pid].title +"</h2>" +
	"<img width='254' height='355' src='"+baseUrl+fedoraPrefix+pid+fedoraSuffix+"'>" +
	     //"<h4>"+ doc[pid].contrib +"</h4>"+
	     //"<h4>"+ doc[pid].loc +"</h4>"+
	     //"<div class=\"closer\"><a href=\"#\" id=\""+pid+"\" >Close</a></div>" +
	"</div>";
	}else{
	    infoString = "<a href='"+baseUrl+objectPrefix+pid+"'>"+ doc[pid].title+"</a>"+
	     //"<p>"+ doc[pid].contrib +"</p>"+
	     //"<p>"+ doc[pid].loc +"</p>" +
	     //"<div class=\"popper\"><a href=\"#\" id=\""+pid+"\">PopUp</a></div>"+
	"<div id=\"overlay_form_"+pid+"\" style=\"display:inline\">"+
	     //"<h2>" + doc[pid].title +"</h2>" +
	"<img width='254' height='355' src='"+baseUrl+fedoraPrefix+pid+fedoraSuffix+"'>" +
	     //"<h4>"+ doc[pid].contrib +"</h4>"+
	     //"<h4>"+ doc[pid].loc +"</h4>"+
	     //"<div class=\"closer\"><a href=\"#\" id=\""+pid+"\" >Close</a></div>" +
	"</div>";
	}	
    }
	return infoString

	    }

function newbuildInfoWindow(p, title, contrib, loc, desc){

    infoString = "<div style='border-bottom:1px solid black'>" +
	"<h2><a href='http://islandora.usask.ca/fedora/repository/"+p+"'>"+ title +"</a></h2>"+
	"<img width='254' height='355' src='"+baseUrl+fedoraPrefix+p+fedoraSuffix+"'>" +
	"<h4>"+ contrib +"</h4>"+
	"<h4>"+ rows +"</h4>"+
	    "<h4>"+ loc +"</h4>"+
	"</div>";
    
    return infoString
	}



    function buildInfoWindow(doc){

	infoString = "<div style='border-bottom:1px solid black'>" +
	    "<h2><a href='http://islandorasev-dev.usask.ca/fedora/repository/"+doc['PID']+ "'>"+ doc['dc.title']+"</a></h2>"+
	    "<img width='254' height='355' src='http://islandorasev-dev.usask.ca/fedora/repository/"+doc['PID']+"/TN/TN'>" +
	    "<h4>"+ doc['dc.contributor'] +"</h4>"+
	    "<h4>"+ doc['mods_physicalLocation_ms'] +"</h4>"+
	    "</div>";
	
	return infoString
	    }




    function getPosition(loc){
	var commaPos = loc.indexOf(',');
	var coordinatesLat = parseFloat(loc.substring(0, commaPos));
	var coordinatesLong = parseFloat(loc.substring(commaPos + 1, loc.length));
	var myLatlng = new google.maps.LatLng(coordinatesLat, coordinatesLong);
	
	return myLatlng;
    }


    $(document).ready(function(){
	    $(".popper a").click(function(){
		    var pid = "overlay_form_"+ $(this).attr('id');
		    $("#"+pid).show();

		    $("#"+pid).css({
			    left: ($(window).width() - $('#'+pid).width()) / 3,
				top: ($(window).width() - $('#'+pid).width()) / 7,
				position:'absolute',
				'background-color':'#f0f0f0',
				});
		});
	});



    $(document).ready(function(){
	    $(".closer a").click(function(){
		    var pid = "overlay_form_"+ $(this).attr('id');
		    $("#"+pid).hide();
		});
		
	});

}


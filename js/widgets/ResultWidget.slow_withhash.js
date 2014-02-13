(function ($) {
    AjaxSolr.ResultWidget = AjaxSolr.AbstractWidget.extend({
	    afterRequest: function () {

		$(this.target).empty();
		var len = this.manager.response.response.docs.length;
		//doc = this.manager.response.response.docs[0];

		var markers = [];
		var totalResults = 0;
		var locationResults = 0;
		var pinHash = new Object();
		for( var i = 0; i < this.manager.response.response.docs.length; i++ ){
		    if(this.manager.response.response.docs[i].mods_coordinates_p != null){
		        var p = getPosition(String(this.manager.response.response.docs[i].mods_coordinates_p));
		        var key = String(this.manager.response.response.docs[i].mods_coordinates_p);
			var title = this.manager.response.response.docs[i]['dc.title'];
			var windowContent = buildInfoWindow(this.manager.response.response.docs[i]);
			var markerStuff = {"p":p, "content": windowContent, "cnt":1};
		//doc.PID, doc.mods_coordinates_p, doc[dc.subject], dc.title, dc.publisher, dc.contributor, dc.type, mods_physical_location_ms, dc.description


			//Need to use a hash because an array suckz nutz
			var matchs = 0;
			if(pinHash.hasOwnProperty(key)){
			    console.log("match");
			    //console.log(windowContent);
			    pinHash[key].content + windowContent;
			    pinHash[key]["cnt"] = pinHash[key]["cnt"] + 1;
			    console.log(pinHash[key].content);
			    
			} else {
			    //console.log("not match");
			    pinHash[key] = markerStuff;
			}

			if(markers.length == 0){
			    markers.push(markerStuff);
			}
            
			markers.every( function(elem){
				if(elem.p.equals(p)){               
				    elem.cnt = elem.cnt + 1;
				    elem.content = elem.content + windowContent;
				    ;                       //alert(elem.content);
				    return false;

				}else{
				    markers.push(markerStuff);                      
				    return true;
				}
			    });

		    }

		}//End for loop
		var c = 0;
		for (var k in pinHash) {
		    c = c +1;
		    // use hasOwnProperty to filter out keys from the Object.prototype
		    if (pinHash.hasOwnProperty(k)) {
			placeMarker(pinHash[k]['p'], pinHash[k]['content'], pinHash[k]['cnt']);            
			//alert('key is: ' + k + ', value is: ' + h[k]);
			//console.log('key is: ' + k + 'val' + pinHash[k]["content"]);
		    }
		}

		console.log("Total Markers: " + c.toString());
		/*
		locationResults = markers.length;
		markers.forEach( function(elem){
			placeMarker(elem.p, elem.content, elem.cnt);            
		    });
		*/
		/*
		alert(doc.PID);		
		var lat = 52.11679;
		var lng = -106.63452;
		var latlng = new google.maps.LatLng(lat,lng);
		var marker = new google.maps.Marker({
			position: latlng,
			map: map,
			title: ' items at this location',
		    });
		*/

	    }
	});
})(jQuery);

var infowindow = new google.maps.InfoWindow({
	size: new google.maps.Size(150,150)
    });


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

}

function buildInfoWindow(doc){

    //    http://dev-islandora.usask.ca/fedora/repository/wdmb%3A1
    var infoString = "";
        infoString = "<div style='border-bottom:1px solid black'>" +
            "<h2><a href='http://dev-islandora.usask.ca/fedora/repository/"+doc['PID']+ "'>"+ doc['dc.title']+"</a></h2>"+
            "<img width='254' height='355' src='http://dev-islandora.usask.ca/fedora/repository/"+doc['PID']+"/TN/TN'>" +
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



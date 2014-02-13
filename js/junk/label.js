// Define the overlay, derived from google.maps.OverlayView
function Label(opt_options) {
// Initialization
 this.setValues(opt_options);

 // Label specific
 var span = this.span_ = document.createElement('span');
 span.style.cssText = 'position: relative; left: -50%; top: -8px; ' +
                      'white-space: nowrap; border: 1px solid blue; ' +
                      'padding: 2px; background-color: white';

 var div = this.div_ = document.createElement('div');
 div.appendChild(span);
 div.style.cssText = 'position: absolute; display: none';
};
Label.prototype = new google.maps.OverlayView;

// Implement onAdd
Label.prototype.onAdd = function() {
 var pane = this.getPanes().overlayLayer;
 pane.appendChild(this.div_);

 // Ensures the label is redrawn if the text or position is changed.
 var me = this;
 this.listeners_ = [
   google.maps.event.addListener(this, 'position_changed',
       function() { me.draw(); }),
   google.maps.event.addListener(this, 'text_changed',
       function() { me.draw(); })
 ];
};

// Implement onRemove
Label.prototype.onRemove = function() {
 this.div_.parentNode.removeChild(this.div_);

 // Label is removed from the map, stop updating its position/text.
 for (var i = 0, I = this.listeners_.length; i < I; ++i) {
   google.maps.event.removeListener(this.listeners_[i]);
 }
};

// Implement draw
Label.prototype.draw = function() {
 var projection = this.getProjection();
 var position = projection.fromLatLngToDivPixel(this.get('position'));

 var div = this.div_;
 div.style.left = position.x + 'px';
 div.style.top = position.y + 'px';
 div.style.display = 'block';

 this.span_.innerHTML = this.get('text').toString();
};


<html>
<head>
 <meta http-equiv="content-type" content="text/html; charset=UTF-8"/>
 <title>Label Overlay Example</title>
 <script type="text/javascript" src="http://maps.google.com/maps/api/js?sensor=false"></script>
 <script type="text/javascript" src="label.js"></script>
 <script type="text/javascript">
   function initialize() {
     var latLng = new google.maps.LatLng(40, -100);

     var map = new google.maps.Map(document.getElementById('map_canvas'), {
       zoom: 5,
       center: latLng,
       mapTypeId: google.maps.MapTypeId.ROADMAP
     });

     var marker = new google.maps.Marker({
       position: latLng,
       draggable: true,
       map: map
     });

     var label = new Label({
       map: map
     });
     label.bindTo('position', marker, 'position');
     label.bindTo('text', marker, 'position');
   };
 </script>
</head>
<body onload="initialize()">
 <div id="map_canvas" style="height: 100%; width: 100%"></div>
</body>
</html>
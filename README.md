islandora_map_browse
====================

Map browser for islandora

__Requirements__

islandora_solr_search
drupal libraries module
drupal colorbox module
drupal chaos tools module

leaflet and Leaflet.markercluster-master needs to be in the libraries directory

To enable islandora map browse you will need to configure gsearch to index mods so a lat lon location fields is available

__Configurations for the module__

islandora_solr_search
	3 New secondary display profiles are made available in the 'Solr Index' configuration_
	'View All Results on Map' - show the entire search results on the map
	'Paged Map View' - Show the current pages results

Selecting one of these adds an icon to the search results that links to a map view

islandora_map_browse
	There are several settings here that need to be included.
	Fedora Base URL - uum, the base url?
	Fedora base prefix - everything before the PID
	Fedora base thumbnail suffix - everything after the PID for the thumbnail view
	Solr coordinate field - the solr field containing the lat, long
	Solr Title field - the solr field containing the title
	Fields to return from solr - these are shown below the thumbnail on the map popup
	Default map centre - the center of the world as you know it.
	Maximum items to render - for safety, returning extra items is not yet handled

Once these are set up you should get a map.  If not check out the solr document that is returned as ensure that the field names are correct

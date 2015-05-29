# Islandora Map Browse

A map browser leveraging the leaflet javascript library and MODS metadata to generate maps based on search results for Islandora.

## Dependencies

Modules
* [Islandora Solr Search](http://github.com/islandora/islandora_solr_search)

Libraries
* [Leaflet.js - v. 0.7.3](http://leaflet-cdn.s3.amazonaws.com/build/leaflet-0.7.3.zip)
   * create a directory called leaflet in sites/all/libraries
   * cd to the leaflet directory, download the zip file and unzip it
* [Leaflet Marker Cluster](https://github.com/Leaflet/Leaflet.markercluster.git)
   * install in sites/all/libraries/Leaflet.markercluster-master

## Configuration

To enable Islandora Map Browse you will need to configure gsearch to index mods so a lat lon location fields is available.  If you are using the standard islandora VM, then you should have a Solr field that is <mods_subject_cartographics_coordinates_ms> and should contain the latitutde,longitude coordinate data.

### Configuring Islandora Map Browse

There are several configuration settings you should review and the module configuration is available on this drupal path - admin/islandora/mapbrowse.

* Fedora Base URL - uum, the base url? 
* Fedora base prefix - everything before the PID
* Fedora base thumbnail suffix - everything after the PID for the thumbnail view
* Solr coordinate field - the solr field containing the lat, long
* Solr Title field - the solr field containing the title
* Fields to return from solr - these are shown below the thumbnail on the map popup
* Default map centre - the center of the world as you know it.
* Maximum items to render - for safety, returning extra items is not yet handled  

![Configuration Options](https://raw.githubusercontent.com/dmoses/islandora_screenshots/master/islandora_map_browse_config.jpg "Configuration Options")


### Configuring Islandora Solr Search

After enabling the Islandora Map Browse module two new secondary display profiles are made available in the Solr Index configuration.

* **View All Results on Map** which displays all the search results on a map
* **Paged Map View** which display the current search results page on a map

Selecting one of these secondary displays adds a folded map icon to the search results. Selecting the icon launches the map display.

Once these are set up you should get a map.  If not check out the solr document that is returned as ensure that the field names are correct.

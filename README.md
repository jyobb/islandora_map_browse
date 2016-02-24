# Islandora Map Browse

A map browser leveraging the leaflet javascript library and MODS metadata to generate maps based on search results for Islandora.

## Dependencies

Modules
* [Islandora Solr Search](http://github.com/islandora/islandora_solr_search)

* [Colorbox](https://www.drupal.org/project/colorbox)



Libraries
* [Leaflet.js - v. 0.7.7](http://leafletjs.com/download.html)
   * create a directory called leaflet in sites/all/libraries
   * cd to the leaflet directory, download the zip file and unzip it

* [Leaflet Marker Cluster](https://github.com/Leaflet/Leaflet.markercluster/tree/leaflet-0.7)
   * install in sites/all/libraries/Leaflet.markercluster

## Configuration

To enable Islandora Map Browse you will need to configure gsearch to index mods so a lat lon location fields is available.  If you are using the standard islandora VM, then you should have a Solr field that is <mods_subject_cartographics_coordinates_ms> and should contain the latitutde,longitude coordinate data.

Before Browse Map will work you need to generate a static map file.  This is done to avoid having a new solr query run everytime someone browses the collection as this was found to be slow for large collections.

### Configuring Islandora Map Browse

There are several configuration settings you should review and the module configuration is available on this drupal path - admin/islandora/mapbrowse.

* Islandora Base URL - http://yourFedoraHostname.ca
* Islandora base prefix - everything before the PID
* Fedora base thumbnail suffix - everything after the PID for the thumbnail view
* Solr coordinate field - the solr field containing the lat, long
* Solr Title field - the solr field containing the title
* Fields to return from solr - these are shown below the thumbnail on the map popup. If a field does not exist for a record it is ignored. Fields need to be added in the following format: Field Name1, field.solr1 | Field Name2, field.solr2. Commas separate Field names from solr fields, | separate each field.
* Default map centre - the center of the world as you know it.
* Maximum items to render - for safety, returning extra items is not yet handled  

![Configuration Options](https://raw.githubusercontent.com/jyobb/islandora_map_browse/master/islandora_map_browse_config.png "Configuration Options")


### Configuring Islandora Solr Search

After enabling the Islandora Map Browse module two new secondary display profiles are made available in the Solr Index configuration.

* **View all search results on map** which displays all the search results on a map
* **Single paged map view** which display the current search results page on a map

Selecting one of these secondary displays adds a folded map icon to the search results. Selecting the icon launches the map display.

Once these are set up you should get a map.  If not check out the solr document that is returned as ensure that the field names are correct.

### Configuration of Colorbox

* Enable Colorbox load 
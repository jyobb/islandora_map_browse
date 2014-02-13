<?php

/**
 * Page callback: islandora_map_browse
 *
 * see islandora_map_browse_menu()
 */

function map_browse_config($form, &$form_state) {

//http://islandorasev-dev.usask.ca/islandora/search/mods_subject_cartographics_coordinates_mt:[* TO *%?type=edismax&solr_profile=MapView

$form['map_browse_all_link'] = array(
  			     
  '#markup' => "<label>Browse Repository URL</label>".variable_get('map_browse_fedora_base')."/islandora/search/".variable_get('map_browse_coord_field'). ":&#91;*&nbsp;TO&nbsp;*&#93;?type=edismax&solr_profile=MapView",
);

  $form['map_browse_fedora_base'] = array(
    '#type' => 'textfield',
    '#title' => t('Fedora base URL'),
    '#default_value' => variable_get('map_browse_fedora_base', 'http://yourFedoraHostname.ca'),
    '#size' => 80,
    '#maxlength' => 256,
    '#description' => t('The internet accessable address of your fedora server'),
    '#required' => TRUE,
  );

  $form['map_browse_fedora_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Fedora base prefix'),
    '#default_value' => variable_get('map_browse_fedora_prefix', '/islandora/object/'),
    '#size' => 60,
    '#maxlength' => 256,
    '#description' => t('Path from hostname to PID'),
    '#required' => TRUE,
  );

  $form['map_browse_fedora_suffix'] = array(
    '#type' => 'textfield',
    '#title' => t('Fedora base Thumbnail suffix'),
    '#default_value' => variable_get('map_browse_fedora_suffix', '/datastream/TN/view'),
    '#size' => 60,
    '#maxlength' => 256,
    '#description' => t('Path after PID'),
    '#required' => TRUE,
  );

  $form['map_browse_coord_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Solr Coordinate Field'),
    '#default_value' => variable_get('map_browse_coord_field', 'mods_subject_cartographics_coordinates_ms'),
    '#size' => 40,
    '#maxlength' => 128,
    '#description' => t('MODS field containing coordinates in Latitude/Longitude format'),
    '#required' => TRUE,
  );

  $form['map_browse_title_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Solr Title Field'),
    '#default_value' => variable_get('map_browse_title_field', 'dc.title'),
    '#size' => 40,
    '#maxlength' => 128,
    '#description' => t('MODS field containing title'),
    '#required' => TRUE,
  );
/*
  $form['map_browse_return_area'] = array(
    '#type' => 'textarea',
    '#title' => t('Solr Title Field'),
    '#default_value' => variable_get('map_browse_return_area'),
    '#cols' => 40,
    '#rows' => 8,
    '#description' => t('Fields and titles to display below item separated by ~ '),
    '#required' => TRUE,
  );
*/


  $form['map_browse_return_fields'] = array(
    '#type' => 'textfield',
    '#title' => t('Fields to Return From Solr'),
    '#default_value' => variable_get('map_browse_return_fields', 'PID, dc.subject, dc.description, dc.contributor, dc.publisher, dc.date, dc.type, dc.source_s, mods_location_physicalLocation_ms, mods_subject_cartographics_coordinates_ms, mods_location_url_ms, RELS_EXT_hasModel_uri_ms'),
    '#size' => 128,
    '#maxlength' => 512,
    '#description' => t('Fields to be displayed on the map points'),
    '#required' => TRUE,
  );

  $form['map_browse_map_centre'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Map Centre'),
    '#default_value' => variable_get('map_browse_map_centre', '52.1311, -106.6353'),
    '#size' => 40,
    '#maxlength' => 128,
    '#description' => t('Centre of the Universe'),
    '#required' => TRUE,
  );

  $form['map_browse_max_rows_to_render'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum items to render'),
    '#default_value' => variable_get('map_browse_max_row_to_render','2000'),
    '#size' => 6,
    '#maxlength' => 12,
    '#description' => t('This can impact performance'),
    '#required' => TRUE,
  );

   $cluster_type = array(0 => t('Item Clustering'));
   //, 1 => t('Location Clustering'));

   $form['map_browse_clustering'] = array(
    '#type' => 'radios',
    '#title' => t('Clustering Model'),
    '#default_value' => 0,
    '#options' => $cluster_type,
    '#description' => t('Cluster by locations or items at a location.  Currently not functioning.'),
    
  );

  return system_settings_form($form);

}
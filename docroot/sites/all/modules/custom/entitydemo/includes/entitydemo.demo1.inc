<?php
/**
 * @file
 * Demonstrate usage of API functions for interacting with existing entities.
 */

function entitydemo_demo_1() {
  $output = array();
  // Given an $id and $type, load an entity.
  $id = 75;
  $types = array('node', 'user', 'comment');
  foreach ($types as $entity_type) {
    // Load the entity of a specific type with the ID of 5.
    $entity = entity_load($entity_type, array($id));

    // We're only retrieving a single entity, however entity_load is capable
    // of loading many and will always return an array so we just grab the
    // only element out of the array here.
    $entity = reset($entity);

    // Determine the entity type.
    // Using $entity->type property is a common convention and you should feel
    // pretty safe using that.
    $output[] = array(
      '#markup' => t('<p>The entity type is: %type</p>', array('%type' => $entity_type)),
    );

    // Determine more information.
    // Other informatino however isn't necessarily obvious. For example on a
    // node entity the ID is $entity->nid, but on a user entity the ID is
    // $entity->uid.
    // This snippet is a handy way to retrieve the ID, Revision ID, and Bundle
    // if it exists, for any entity.
    list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

    $output[] = array(
      '#markup' => t('<p>ID: %id, Revision: %vid, Bundel: %bundle</p>', array('%id' => $id, '%vid' => $vid, '%bundle' => $bundle)),
    );

    // Dump it to the screen or inspection.
    $output[] = array(
      '#markup' => kpr($entity, TRUE),
    );
  }

  $video = entity_load_single('video', 4);
  $wrapper = entity_metadata_wrapper('video', $video);
  dsm($wrapper->duration->value());

  return $output;
}

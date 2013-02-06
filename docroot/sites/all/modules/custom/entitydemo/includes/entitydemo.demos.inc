<?php
/**
 * @file
 * List of entity demo's provided by this module.
 */

/**
 * Page callback. Create a list of demo's and display links to each one.
 */
function entitydemo_demos() {
  $links = array();

  // We just loop through everything in the module's hook_menu implementation
  // and print that list here.
  $items = entitydemo_menu();
  unset($items['demo']);
  foreach ($items as $url => $item) {
    $links[] = l($item['title'], $url) . ' : ' . $item['description'];
  }

  if (empty($links)) {
    $links[] = t('Doesn\'t look like any demos have been setup yet.');
  }

  return theme('item_list', array('items' => $links));
}

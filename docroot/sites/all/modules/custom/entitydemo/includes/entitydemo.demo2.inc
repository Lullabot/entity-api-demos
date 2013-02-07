<?php
/**
 * @file
 * Demonstrate use of EntityFieldQuery.
 */

function entitydemo_demo_2() {
  $output = array();
  // Build a list of all entities of a given type. This is a super simple
  // example.

  // Create a new EFQ object.
  $query = new EntityFieldQuery();
  // Use the entityCondition() method to select based on meta data that most all
  // entities will contain. Things like ID, or Revision ID, but which may have
  // entity specific names for the property such as {node}.nid vs {user}.uid.
  // The entityCondition() method takes three arguments.
  // the name of the property, the desired value to check for, and an optional
  // operator such as '>' or '<=' which defaults to '=' unless the value is an
  // array in which case it will default to 'IN'. Most standard SQL operators
  // will work here.
  $query->entityCondition('entity_type', 'node');
  // Execute the query.
  $result = $query->execute();

  $output[] = array(
    '#prefix' => t('<h2>entityCondition() Query Result</h2>'),
    '#markup' => kpr($result, TRUE),
  );


  // Example using a fieldCondition() method. Note that this query will also
  // potentially return multiple types of entities since a field can have many
  // instances and be attached to any bundle.
  $query = new EntityFieldQuery();
  // Query the 'field_number' field which is applied to both the 'article' and
  // 'user' bundles.
  // The fieldCondition() method takes 4 arguments.
  // @field = which is the machine name of the field.
  // @column = the name of the column for a field to inspect for values. Since
  //   some field types have multiple columns in the database. Images for
  //   example store the file ID along with alt & title text. The easiest way
  //   to know what the possible columns are for a field are to look up the
  //   hook_field_schema definition for that field which defines the columns for
  //   the field type. You can also just look at the database and find the
  //   corresponding {field_data_*} table and look at the column names there.
  //   However, note that the column name in the DB will be something like
  //   field_number_value or field_image_alt with the first part being the
  //   machine reable field name. Only the part after the field name is used in
  //   the @column argument.
  // @value = the value which you would like to filter for.
  // @operator = an optional operator to use.
  $query->fieldCondition('field_number', 'value', 100, '>');

  $result = $query->execute();
  $output[] = array(
    '#prefix' => t('<h2>fieldCondtition() Query Result</h2>'),
    '#markup' => kpr($result, TRUE),
  );

  $output[] = array(
    '#prefix' => t('<h2>Defined fieldCondtion() columns</h2>'),
    '#markup' => _entitydemo_columns_helper(),
  );

  // An example of using the propertyCondition() method.
  $query = new EntityFieldQuery();
  // Note that the propertyCondition() method requires that you specify an
  // entity_type condition since properties are entity type specific. For
  // example nodes have the 'promote' property that indicates a node is promoted
  // to the front page. Users however have no such property. Failure to specify
  // an entity_type condition will throw an 'EntityFieldQueryException'.
  $query->entityCondition('entity_type', 'node');
  // The propertyCondition() method allows you to filter the results of a query
  // based on properties. This generally maps to the columns in the base table
  // of an entity.
  $query->propertyCondition('promote', 1);

  $result = $query->execute();
  $output[] = array(
    '#prefix' => t('<h2>Property Condition Query</h2>'),
    '#markup' => kpr($result, TRUE),
  );

  $output[] = array(
    '#prefix' => t('<h2>Defined properties</h2>'),
    '#markup' => _entitydemo_properties_helper(),
  );

  // Limiting results.
  $query = new EntityFieldQuery();
  $query->fieldCondition('field_number', 'value', 100, '>');
  $query_copy = $query;
  // You can use the range() method to limit results. It takes two parameters
  // start and end. Note that for queries which return results from multiple
  // entity types using range() can be confusing. It does NOT set a range per
  // type but rather in total. And thus if your query, like the one here, could
  // result in 15 nodes, and 10 users being returned you'll only get 20 results
  // total which will equate to all 15 nodes but just the first 5 users.
  $query->range(0, 20);

  $result = $query->execute();
  $output[] = array(
    '#prefix' => t('<h2>Limited Range Query</h2>'),
    '#markup' => kpr($result, TRUE),
  );

  $query = $query_copy;
  // Alternately we can use a pager to allow users to page through the results
  // from our query. Using the same query as above but without the range()
  // specified.
  // Simply invoke the pager() method with an argument that is the number of
  // items to return per page.
  $query->pager(10);

  // The results are output exactly the same.
  $result = $query->execute();
  $output[] = array(
    '#prefix' => t('<h2>Limited Range Query w/ Pager</h2>'),
    '#markup' => kpr($result, TRUE),
  );
  // We just need to make sure an include a pager element on the page.
  $output[] = array('#theme' => 'pager');

  // Handling EFQ results.
  $result_copy = $result;
  $output[] = array(
    '#prefix' => t('<h2>A single EFQ result</h2>'),
    '#markup' => kpr(array_shift($result_copy['node']), TRUE),
  );
  // The array of results returned from EFQ only contains the meta data for an
  // entity. ID, Revision, and TYPE for each entity found by the query. In
  // addition this information is nested inside the $result array in a sub-array
  // keyed by entity TYPE. This is necessary since some queries may return
  // entities of more than one type. This way they are grouped together by type.
  // For example.
  // $results = array(
  //   'node' => array(...),
  //   'user' => array(...),
  // );
  // Where 'node', and 'user' are entity types. Each type key contains an array
  // of entity meta data values keyed by the entity ID.
  // Knowing this we can do the following.

  // Load the full node object for all the nodes returned by the query. This is
  // done by getting an array of the returned IDs and then using entity_load().
  // This is considered best practice.
  $nids = array_keys($result['node']);
  $nodes = entity_load('node', $nids);
  $output[] = array(
    '#prefix' => t('<h2>All nodes</h2>'),
    '#markup' => kpr($nodes, TRUE),
  );

  return $output;
}

function _entitydemo_columns_helper() {
  // Load all include .install files.
  module_load_all_includes('install');
  $fields = field_info_field_types();
  $schemas = array();
  foreach ($fields as $type => $field) {
    $field['type'] = $type;
    $function = $field['module'] . '_field_schema';
    $schemas[$type] = $function($field);
  }

  $headers = array(t('Field Type'), t('Columns'));
  $rows = array();
  foreach ($schemas as $key => $value) {
    $columns = array();
    foreach ($value['columns'] as $n => $s) {
      $columns[] = $n . ' (' . $s['type'] . ')';
    }
    $rows[] = array(
      $key,
      implode(', ', $columns),
    );
  }
  return theme('table', array('header' => $headers, 'rows' => $rows));
}

function _entitydemo_properties_helper() {
  foreach(module_implements('entity_info') as $module) {
    $info = module_invoke($module, 'entity_info');
    $schema = module_invoke($module, 'schema');
    foreach (array_keys($info) as $entity_type) {
      $rows[] = array(
        $entity_type,
        implode(', ', array_keys($schema[$info[$entity_type]['base table']]['fields'])),
      );
    }
  }
  $headers = array(t('Entity Type'), t('Properties'));
  return theme('table', array('header' => $headers, 'rows' => $rows));
}
<?php
/**
 * @file
 * Demonstrate use of EntityFieldQuery.
 *
 * Why EntityFieldQuery? Why not just query the database directly and get what
 * I need? I already know SQL/DBTNG why should I learn something else?
 *
 * Because the underlying architecture which you're querying against can change
 * via the click of a button in the UI. With tools like Field UI it's possible
 * for an administrator or even another module to make changes to your
 * underlying schema which can easily break your custom SQL query.
 *
 * EFQ makes it easy to query for entities based on their properties, their
 * meta data, and any fields attached to their respective bundles. And it really
 * starts to shine when querying for field values. Now that fields can be
 * applied & shared between many different entity types the legwork one would
 * need to do with SQL joins just to get the ID of all entities of any type
 * that have a value for a specified field is ... daunting to say the least. And
 * again, an admin could add the field to another entity type via the UI and
 * suddenly you're having to update your module.
 *
 * Need more reasons?
 * - EFQ respects read permissions.
 * - EFQ rides a unicorn.
 *
 */

function entitydemo_demo_2() {
  $output = array();
  // Build a list of all entities of a given type. This is a super simple
  // example.

  // --------------------------------------------------------------------------
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


  // --------------------------------------------------------------------------
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

  // --------------------------------------------------------------------------
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

  // --------------------------------------------------------------------------
  // Ordering results.
  $query = new EntityFieldQuery();
  $query->fieldCondition('field_number', 'value', 100, '>');
  // The results returned by EFQ can be ordered using any of the three methods
  // entityOrderBy(), propertyOrderBy() and fieldOrderBy(). Which like their
  // respective condition methods work on meta data, properties, and fields
  // respectively. All three take an argument specifying which
  // data/property/field to order on and a direction of either 'ASC' or 'DESC'.
  // The fieldOrderBy() method also expects a field name argument. The field
  // name and column arguments work just like those for the fieldCondition()
  // method.
  //
  // You can also chain multiple orderBy methods and they will influence sort
  // ordering in the order they are called.
  //
  // Note: that propertyOrderBy() might not apply in all cases since properties
  // are unique to entity types and EFQ will throw an exception if you try and
  // use propertyOrderBy() without an entity_type condition.
  // $query->entityCondition('entity_type', 'node');
  $query->fieldOrderBy('field_number', 'value', 'DESC');
  $query_copy = $query;

  // Note that results are still returned in as an array of arrays where the
  // outer key is the entity type and the inner arrays are the stub entities.
  $result = $query->execute();
  $output[] = array(
    '#prefix' => t('<h2>Ordering Results</h2>'),
    '#markup' => kpr($result, TRUE),
  );

  // In theory you can use $query->orderedResults in this scenario to get all
  // entities regardless of type in the appropriate order however I have not
  // been able to get that to ever actually contain a value. :/

  // Note that ordering by a field will limit the result to ONLY entities that
  // have that field. For example the image field here is only on node/article
  // entities and not users so users are excluded from the returned results.
  $query = $query_copy;
  $query->fieldOrderBy('field_image', 'alt');

  $result = $query->execute();
  $output[] = array(
    '#prefix' => t('<h2>Ordering Results w/ field_image</h2>'),
    '#markup' => kpr($result, TRUE),
  );

  // --------------------------------------------------------------------------
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

  // Alternately we can use a pager to allow users to page through the results
  // from our query. Using the same query as above but without the range()
  // specified.
  // Simply invoke the pager() method with an argument that is the number of
  // items to return per page.
  $query = $query_copy;
  $query->pager(10);

  // The results are output exactly the same.
  $result = $query->execute();
  $output[] = array(
    '#prefix' => t('<h2>Limited Range Query w/ Pager</h2>'),
    '#markup' => kpr($result, TRUE),
  );
  // We just need to make sure an include a pager element on the page.
  $output[] = array('#theme' => 'pager');

  // --------------------------------------------------------------------------
  // Tags
  // You can use the addTag() method to add additional tags to your EFQ queries
  // which are then accessible to hook_query_alter() and hook_query_TAG_alter()
  // implementations.
  //
  // This is important because by EFQ queries are run through node access
  // checks if they contain a fieldCondition() and only entities that the
  // current user has permission to view are returned. Generally this is good
  // behaviour. However there are legitimate use cases for wanting to retrieve
  // all results regardless of access control. This example demonstrates using
  // the built in 'DANGEROUS_ACCESS_CHECK_OPT_OUT' tag to perform a query and
  // bypass all access checks.
  $query = new EntityFieldQuery();
  $query->fieldCondition('field_number', 'value', 100, '>')
    ->addTag('DANGEROUS_ACCESS_CHECK_OPT_OUT');

  // Another example would be adding a random sort order to EFQ.
  // Add the tag, and then define a function which alters the SQL query
  // object and adds a random sort.
  // @see entitydemo_query_random_alter().
  $query = new EntityFieldQuery();
  $query->fieldCondition('field_number', 'value', 100, '>')
    ->addTag('random')
    ->range(0, 10);

  $result = $query->execute();
  $output[] = array(
    '#prefix' => t('<h2>Random sort order via addTag()</h2>'),
    '#markup' => kpr($result, TRUE),
  );

  // --------------------------------------------------------------------------
  // Handling EFQ results.
  // EFQ returns only stub-entites. The ID, Revision ID, and TYPE. Or ...
  // exactly what we need to load the full entity using the entity_load()
  // function!
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

  // --------------------------------------------------------------------------
  // Practical example.
  // Combine it all together and you can do something like this, which retrieves
  // all article nodes posted in the last 6 months that are promoted to the
  // front page, ordered by the value in the field_number field.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'article')
    ->propertyCondition('created', (strtotime('-6 months', REQUEST_TIME)), '>=')
    ->fieldOrderBy('field_number', 'value');

  $result = $query->execute();
  $output[] = array(
    '#prefix' => t('<h2>All article nodes from the last 6 months ordered by field_number</h2>'),
    '#markup' => kpr($result, TRUE),
  );

  // --------------------------------------------------------------------------
  // Extending EFQ
  //
  // Remember that EFQ is just a simple PHP class. Which means that like any
  // other class we can extend it with our own variant.
  //
  // For example, lets say we're dealing with code queries that will always
  // return nodes, that are published and are authored by the current user.
  // We're not worried about other entity types in our module, but we do want
  // to be able to add additional conditions. You can save your self a lot of
  // typing by extending the EFQ class and adding these always present options
  // to the constructor.

  // Create a new query object from our custom class.
  $query = new EntityDemoFieldQuery();
  // Call our custom method to limit to the current user's nodes.
  $query->currentUserCondition();

  // Add an order by using regular old EFQ methods.
  //$query->propertyOrderBy('created', 'DESC');

  $result = $query->execute();
  $output[] = array(
    '#prefix' => t('<h2>Result from EntityDemoFieldQuery</h2>'),
    '#markup' => kpr($result, TRUE),
  );

  return $output;
}

/**
 *  An example extension of the EntityFieldQuery class.
 */
class EntityDemoFieldQuery extends EntityFieldQuery {

  public function __construct() {
    // Limit all queries using this class to node entities.
    $this->entityCondition('entity_type', 'node');
    // That are published.
    $this->propertyCondition('status', 1);
  }

  public function currentUserCondition() {
    global $user;
    $this->propertyCondition('uid', $user->uid);
  }

}

/**
 * Helper function to build a table of possible column values for the enabled
 * field types.
 */
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

/**
 * Helper function to build a list of properties from the base tables of all
 * all enabled entity types.
 */
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

# Entity API Demo

This repository houses the code used/created during the [Drupalize.Me](htp://drupalize.me) series covering the Entity API in Drupal 7.

### Use

… fill this in ...

**Creating New DB Snapshots**

````
drush sql-dump | gzip > ../db/{CHAPTER}.sql.gz
````

This assumes you're using the drush/drushrc.php file which has settings for excluding all the cache_* and a few other tables.
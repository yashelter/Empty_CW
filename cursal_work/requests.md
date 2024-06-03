# Типы запросов - параметры

1. /add_pool - pool_name

2. /remove_pool - pool_name

3. /add_scheme - pool_name scheme_name

4. /remove_scheme - pool_name scheme_name

5. /add_collection - pool_name scheme_name collection_name

6. /remove_collection - pool_name scheme_name collection_name

7. /insert - pool_name scheme_name collection_name data json(key:string, value:student)

8. /read_value - pool_name scheme_name collection_name need_persist:bool (time_point)  data  json(key:string)

9. /read_range - pool_name scheme_name collection_name need_persist:bool (time_point) data  json(lower:string, upper:string)

10. /update - pool_name scheme_name collection_name  data  json(key:string, value:student)

11. /get - data json(id:CW_GUID)

12. /heart

# Типы ответов

1 - 10 - json(id:GUID)

11 - json(status:bool, json)

    1-7, 10 - message:string
    8 - data:student or message with error
    9 - data:[{key:string, value:student}, ...] or message with error
# Battery Logger
This is a simple battery logger for [AsteroidOS](http://asteroidos.org/)

It works by first looking into the `/sys/class/power_supply` directory and searching there for a directory that contains both a `present` file with the contents `1` (indicating that the device is present) a `type` with the contents "Battery" (indicating a battery device) and  a `voltage_now` file.  Once such a directory is found, this app will log the following quantities, if they exist:

 - voltage_now
 - current_now
 - temp
 - capacity
 - state
 - path

The quantities, with a corresponding timestamp are logged to a sqlite3 database.  Any numeric value not present will be logged as 0.0, and if the `state` is not present, it will be logged as "Unknown". 

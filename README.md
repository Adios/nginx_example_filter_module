an nginx http filter module skeleton.  
  
activate with 
  
    location /test {
        example on; # *location* only.
    }
  
it just prints out what it finds on buffer chain into debug log, as it receive an HTTP POST request with specific content-type. 

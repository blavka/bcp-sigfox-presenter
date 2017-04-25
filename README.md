# BigClown bcp-sigfox-presenter

* Clone reository
    ```
    git clone --recursive
    ```

* Make firmware
    ```
    make release
    ```

* Install [localtunel](https://localtunnel.me)
	```
	sudo npm install -g localtunnel
	```

* Install Pyhton3 packages
	```
	sudo pip3 install -r requirements.txt
	```

* Run server
    ```
    .server.py
    ```

* Run localtunnel
    ```
    lt --port 8080
    ```

    Or run with own subdomain
    ```
    lt --port 8080 --subdomain bcpsigfoxpresenter
    ```

* Configure http callback in sigfox

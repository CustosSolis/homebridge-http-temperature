var Service, Characteristic;
var request = require('sync-request');

var temperatureService;
var url
var temperature = 0;

module.exports = function (homebridge) {
    Service = homebridge.hap.Service;
    Characteristic = homebridge.hap.Characteristic;
    homebridge.registerAccessory("homebridge-http-temperature", "HttpTemperature", HttpTemperature);
}

function HttpTemperature(log, config) {
    this.log = log;

    // url info
    this.url = config["url"];
    this.http_method = config["http_method"] || "GET";
    this.sendimmediately = config["sendimmediately"] || "";
    this.name = config["name"];
    this.manufacturer = config["manufacturer"] || "Dallas";
    this.model = config["model"] || "DS18B20";
    this.serial = config["serial"] || "None";
}

HttpTemperature.prototype = {

    httpRequest: function (url, body, method, username, password, sendimmediately, callback) {
        request({
                    url: url,
                    body: body,
                    method: method,
                    rejectUnauthorized: false
                },
                function (error, response, body) {
                    callback(error, response, body)
                })
    },

    getState: function (callback) {
        var body;

	var res = request(this.http_method, this.url, {});

	if (res.statusCode > 400) {
	  this.log('HTTP power function failed');
	  callback(error);
	} else {
	  this.log('HTTP power function succeeded!');
          var info = JSON.parse(res.body);

          temperatureService.setCharacteristic(Characteristic.CurrentTemperature, info.temperature);

          this.log(res.body);
          this.log(info);

          this.temperature = info.temperature;

	  callback(null, this.temperature);
	}
    },

    identify: function (callback) {
        this.log("Identify requested!");
        callback(); // success
    },

    getServices: function () {
        var services = [],
            informationService = new Service.AccessoryInformation();

        informationService
                .setCharacteristic(Characteristic.Manufacturer, this.manufacturer)
                .setCharacteristic(Characteristic.Model, this.model)
                .setCharacteristic(Characteristic.SerialNumber, this.serial);
        services.push(informationService);

        temperatureService = new Service.TemperatureSensor(this.name);
        temperatureService
                .getCharacteristic(Characteristic.CurrentTemperature)
                .on('get', this.getState.bind(this));
        services.push(temperatureService);

        return services;
    }
};

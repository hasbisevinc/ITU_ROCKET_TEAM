function DatabaseEntity() {
    this.realTime;
    this.packetId;
    this.time;
    this.temperature;
    this.pressure;
    this.altitude;
    this.state;
    this.lang;
    this.long;

   this.create = function(realTime, packetId, time, temperature, pressure, altitude, state, lang, long) {
        this.realTime = realTime;
        this.packetId = packetId;
        this.time = time;
        this.temperature = temperature;
        this.pressure = pressure;
        this.altitude = altitude;
        this.state = state;
        this.lang = lang;
        this.long = long;
    }
}
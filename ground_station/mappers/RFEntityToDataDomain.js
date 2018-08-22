function RFEntityToDataDomain() {
    
    var getData = function(startChar, endChar, message) {
        if (message.lastIndexOf(startChar) == -1 || message.lastIndexOf(endChar) == -1) {
            return "";
        }
        str = message.substring(
            message.lastIndexOf(startChar) + 1, 
            message.lastIndexOf(endChar)
        )
        return str;
    }

    this.map = function(rfEntity) {
        var message = rfEntity.message;
        
        var dataDomain = new DataDomain();
        dataDomain.create();
        
        domain = new DataDomain();
        domain.create(
            new Date(),
            getData("a", "b", message),
            getData("c", "d", message),
            getData("e", "f", message),
            getData("g", "h", message),
            getData("i", "j", message),
            getData("k", "l", message),
            getData("m", "n", message),
            getData("o", "p", message),
            getData("r", "s", message),
            getData("t", "u", message),
            getData("v", "x", message)
        )
        
        /*this.realTime;
        this.packetId;
        this.time;
        this.temperature;
        this.pressure;
        this.altitude;
        this.state;
        this.lat;
        this.long;*/

        return domain;
    }
}
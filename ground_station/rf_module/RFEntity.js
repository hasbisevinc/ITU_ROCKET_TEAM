var RFEntity = function() {
    this.receiveTime;
    this.message;

    this.create = function(receiveTime, message) {
        this.receiveTime = receiveTime;
        this.message = message;
    }
}
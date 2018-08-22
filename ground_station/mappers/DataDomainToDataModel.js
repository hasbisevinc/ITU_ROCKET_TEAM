function DataDomainToDataModel() {
    this.map = function(domain) {
        var dataModel = new DataModel();
        dataModel.create(domain.packetId,
            domain.time,
            domain.temperature,
            domain.pressure,
            domain.acc_x,
            domain.acc_y,
            domain.acc_z,
            domain.altitude,
            domain.state,
            domain.lat,
            domain.long);
        return dataModel;
    }
}
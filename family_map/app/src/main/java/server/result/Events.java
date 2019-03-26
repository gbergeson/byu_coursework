package server.result;

public class Events {
    public Event[] data;

    public server.model.Event[] makeModel() {
        server.model.Event[] modelData = new server.model.Event[data.length];

        for (int i = 0; i < data.length; i++)
            modelData[i] = data[i].makeModel();

        return modelData;
    }
}

package server.result;

public class People extends Result {
    public Person[] data;

    public server.model.Person[] makeModel() {
        server.model.Person[] modelData = new server.model.Person[data.length];

        for (int i = 0; i < data.length; i++)
            modelData[i] = data[i].makeModel();

        return modelData;
    }
}

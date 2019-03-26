package server.result;

public class Person extends Result {
    public String descendant, personID, firstName, lastName, gender, father, mother, spouse;

    public server.model.Person makeModel() {
        return new server.model.Person(personID, descendant, firstName, lastName, gender, father, mother, spouse);
    }
}

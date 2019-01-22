package server.model;

public class Person {
    private String personID, descendant, firstName, lastName, gender, father, mother, spouse, relation;

    public Person(String personID, String descendant, String firstName, String lastName, String gender, String father, String mother, String spouse) {
        this.personID = personID;
        this.descendant = descendant;
        this.firstName = firstName;
        this.lastName = lastName;
        this.gender = gender;
        this.father = father;
        this.mother = mother;
        this.spouse = spouse;
    }

    public void setRelationship(String relation) {
        this.relation = relation;
    }

    public String getRelationship() {
        return relation;
    }

    public void setPersonID(String personID) {
        this.personID = personID;
    }

    public void setDescendant(String descendant) {
        this.descendant = descendant;
    }

    public void setFather(String father) {
        this.father = father;
    }

    public void setFirstName(String firstName) {
        this.firstName = firstName;
    }

    public void setGender(String gender) {
        this.gender = gender;
    }

    public void setLastName(String lastName) {
        this.lastName = lastName;
    }

    public void setSpouse(String spouse) {
        this.spouse = spouse;
    }

    public void setMother(String mother) {
        this.mother = mother;
    }

    public String getSpouse() {
        return spouse;
    }

    public String getPersonID() {
        return personID;
    }

    public String getMother() {
        return mother;
    }

    public String getLastName() {
        return lastName;
    }

    public String getGender() {
        return gender;
    }

    public String getFather() {
        return father;
    }

    public String getFirstName() {
        return firstName;
    }

    public String getDescendant() {
        return descendant;
    }

    public String getFullName() {
        return firstName + " " + lastName;
    }
}

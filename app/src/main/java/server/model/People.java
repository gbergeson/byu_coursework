package server.model;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class People {
    private static People instance = null;
    private People(){}

    public static People getInstance() {
        if (instance == null)
            instance = new People();
        return instance;
    }

    private Person[] data;
    private Set<String> fatherSide;
    private Set<String> motherSide;

    public Person[] getData() {
        return data;
    }

    public void setData(Person[] data) {
        this.data = data;
        fatherSide = new HashSet<>();
        motherSide = new HashSet<>();

        Person user = getPerson(User.getInstance().getPersonId());
        if (user != null) {
            fatherSide.add(user.getPersonID());
            motherSide.add(user.getPersonID());

            getAncestors(user.getFather(), fatherSide);
            getAncestors(user.getMother(), motherSide);
        }
    }

    private void getAncestors(String personID, Set<String> family) {
        if (personID == null)
            return;

        family.add(personID);
        Person person = getPerson(personID);
        getAncestors(person.getFather(), family);
        getAncestors(person.getMother(), family);
    }

    public Person getPerson(String personId) {
        if (personId == null)
            return null;

        for(Person p : data) {
            if (p.getPersonID().equals(personId))
                return p;
        }
        return null;
    }

    public Person[] getFamily(String personId) {
        //This returns the IDs of the parents, the spouse, and the children of the person passed in.

        ArrayList<Person> family = new ArrayList<>();
        Person person = getPerson(personId);

        String father = person.getFather();
        String mother = person.getMother();
        String spouse = person.getSpouse();

        if (father != null) {
            Person dad = getPerson(father);
            dad.setRelationship("Father");
            family.add(dad);
        }
        if (mother != null) {
            Person mom = getPerson(mother);
            mom.setRelationship("Mother");
            family.add(mom);
        }
        if (spouse != null) {
            Person husbWife = getPerson(spouse);
            husbWife.setRelationship("Spouse");
            family.add(husbWife);
        }

        ArrayList<Person> children = getChildren(person);

        family.addAll(children);

        return family.toArray(new Person[family.size()]);
    }

    public ArrayList<Person> getChildren(Person person) {
        String gender = person.getGender();
        String personId = person.getPersonID();
        ArrayList<Person> children = new ArrayList<>();

        if(gender.equals("m")) {
            for (Person p : data)
                if (p.getFather() != null)
                    if (p.getFather().equals(personId)) {
                        p.setRelationship("Child");
                        children.add(p);
                    }
        } else //(gender == "f")
            for (Person p : data)
                if (p.getMother() != null)
                    if (p.getMother().equals(personId)) {
                        p.setRelationship("Child");
                        children.add(p);
                    }

        return children;
    }

    public boolean isMothersSide(String personID) {
        for (String s : motherSide)
            if (personID.equals(s))
                return true;

        return false;
    }

    public boolean isFathersSide(String personID) {
        for (String s : fatherSide)
            if (personID.equals(s))
                return true;

        return false;
    }

    public List<Person> search(String searchValue) {
        ArrayList<Person> results = new ArrayList<>();

        for (Person p : data) {
            if (p.getFullName().toLowerCase().contains(searchValue.toLowerCase()))
                results.add(p);
        }

        return results;
    }

    public void destroy() {
        instance = null;
    }
}

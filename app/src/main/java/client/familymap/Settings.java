package client.familymap;

public class Settings {
    private static Settings instance;
    private int lifeStoryColor, familyTreeColor, spouseColor, mapType;
    private boolean lifeStoryOn, familyTreeOn, spouseOn;

    private Settings() {
        //These values correspond to values in the spinner arrays.
        //0 = green
        //1 = blue
        //2 = red
        lifeStoryColor = 0;
        familyTreeColor = 1;
        spouseColor = 2;
        //0 = Normal
        //1 = Satellite
        //2 = Hybrid
        //3 = Terrain
        mapType = 0;
        lifeStoryOn = false;
        familyTreeOn = false;
        spouseOn = false;
    }

    public static Settings getInstance() {
        if (instance == null)
            instance = new Settings();
        return instance;
    }


    public int getLifeStoryColor() {
        return lifeStoryColor;
    }

    public void setLifeStoryColor(int lifeStoryColor) {
        this.lifeStoryColor = lifeStoryColor;
    }

    public int getFamilyTreeColor() {
        return familyTreeColor;
    }

    public void setFamilyTreeColor(int familyTreeColor) {
        this.familyTreeColor = familyTreeColor;
    }

    public int getSpouseColor() {
        return spouseColor;
    }

    public void setSpouseColor(int spouseColor) {
        this.spouseColor = spouseColor;
    }

    public int getMapType() {
        return mapType;
    }

    public void setMapType(int mapType) {
        this.mapType = mapType;
    }

    public boolean isLifeStoryOn() {
        return lifeStoryOn;
    }

    public void setLifeStoryOn(boolean lifeStoryOn) {
        this.lifeStoryOn = lifeStoryOn;
    }

    public boolean isFamilyTreeOn() {
        return familyTreeOn;
    }

    public void setFamilyTreeOn(boolean familyTreeOn) {
        this.familyTreeOn = familyTreeOn;
    }

    public boolean isSpouseOn() {
        return spouseOn;
    }

    public void setSpouseOn(boolean spouseOn) {
        this.spouseOn = spouseOn;
    }

    public void destroy() {
        instance = null;
    }
}

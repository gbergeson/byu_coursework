package client.familymap;

import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.NavUtils;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

import static com.google.android.gms.maps.model.BitmapDescriptorFactory.HUE_BLUE;
import static com.google.android.gms.maps.model.BitmapDescriptorFactory.HUE_GREEN;
import static com.google.android.gms.maps.model.BitmapDescriptorFactory.HUE_RED;
import static com.google.android.gms.maps.model.BitmapDescriptorFactory.HUE_YELLOW;
import static com.google.android.gms.maps.model.BitmapDescriptorFactory.defaultMarker;

import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;
import com.joanzapata.iconify.Iconify;
import com.joanzapata.iconify.fonts.FontAwesomeModule;
import com.joanzapata.iconify.IconDrawable;
import com.joanzapata.iconify.fonts.FontAwesomeIcons;

import java.util.ArrayList;
import java.util.List;

import server.model.Event;
import server.model.Events;
import server.model.People;
import server.model.Person;
import server.model.User;

public class FMCMapFragment extends Fragment implements OnMapReadyCallback {
    private GoogleMap mMap;
    private TextView mEventText;
    private String mClickedEventID = "";
    private ImageView mEventImage;
    private Settings settings = Settings.getInstance();
    private List<Polyline> polylines = new ArrayList<>();

    public FMCMapFragment() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View inflatedView = inflater.inflate(R.layout.fragment_fmcmap, container, false);

        SupportMapFragment mapFragment = (SupportMapFragment) getChildFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);

        this.setHasOptionsMenu(true);

        mEventText = inflatedView.findViewById(R.id.eventText);
        mEventImage = inflatedView.findViewById(R.id.eventImage);

        mEventText.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mClickedEventID.equals(""))
                    Toast.makeText(getActivity(), "Please select an event.", Toast.LENGTH_SHORT).show();
                else {
                    Intent intent = new Intent(getActivity(), PersonActivity.class);
                    String newPersonID = Events.getInstance().getEvent(mClickedEventID).getPersonID();
                    intent.putExtra("PersonID", newPersonID);
                    startActivity(intent);
                }
            }
        });

        Iconify.with(new FontAwesomeModule());

        return inflatedView;
    }

    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;

        setMapType();
        addMarkers();

        if (getArguments() != null) {
            String clickedEventID = getArguments().getString("EventID");
            server.model.Event clickedEvent = Events.getInstance().getEvent(clickedEventID);

            updateEventInfo(clickedEventID);

            mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(clickedEvent.getLatitude(), clickedEvent.getLongitude()), 3));

            if (settings.isLifeStoryOn())
                drawLifeStoryLines(clickedEventID);
            if (settings.isFamilyTreeOn())
                drawFamilyTreeLines(clickedEventID);
            if (settings.isSpouseOn())
                drawSpouseLines(clickedEventID);
        } else {
            Person userPerson = People.getInstance().getPerson(User.getInstance().getPersonId());
            Event focus = Events.getInstance().getEvents(userPerson.getPersonID())[0];

            mMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(focus.getLatitude(), focus.getLongitude())));
        }
    }

    public void setMapType() {
        int userSetMapType = settings.getMapType();
        switch (userSetMapType) {
            case 0:
                userSetMapType = GoogleMap.MAP_TYPE_NORMAL;
                break;
            case 1:
                userSetMapType = GoogleMap.MAP_TYPE_SATELLITE;
                break;
            case 2:
                userSetMapType = GoogleMap.MAP_TYPE_HYBRID;
                break;
            case 3:
                userSetMapType = GoogleMap.MAP_TYPE_TERRAIN;
                break;
            default:
                userSetMapType = GoogleMap.MAP_TYPE_NORMAL;
        }
        mMap.setMapType(userSetMapType);
    }

    public void addMarkers() {
        for(server.model.Event e : Events.getInstance().getData())
            newMarker(e);
        setMarkerListeners();
    }

    void setMarkerListeners() {
        mMap.setOnMarkerClickListener(new GoogleMap.OnMarkerClickListener() {
            @Override
            public boolean onMarkerClick(Marker marker) {
                clearPolylines();

                String eventID = (String)marker.getTag();

                updateEventInfo(eventID);

                server.model.Event clickedEvent = Events.getInstance().getEvent(eventID);

                updateEventInfo(eventID);

                mMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(clickedEvent.getLatitude(), clickedEvent.getLongitude())));

                if (settings.isLifeStoryOn())
                    drawLifeStoryLines(eventID);
                if (settings.isFamilyTreeOn())
                    drawFamilyTreeLines(eventID);
                if (settings.isSpouseOn())
                    drawSpouseLines(eventID);

                return true;
            }
        });
    }

    private void clearPolylines() {
        for (Polyline line : polylines)
            line.remove();

        polylines.clear();
    }

    private void drawSpouseLines(String eventID) {
        Event event = Events.getInstance().getEvent(eventID);
        Person person = People.getInstance().getPerson(event.getPersonID());

        if (person.getSpouse() != null) {
            Event[] spouseEvents = Events.getInstance().getEvents(person.getSpouse());
            if (spouseEvents != null) {
                LatLng origin = new LatLng(event.getLatitude(), event.getLongitude());
                LatLng dest = new LatLng(spouseEvents[0].getLatitude(), spouseEvents[0].getLongitude());
                Polyline line = mMap.addPolyline(new PolylineOptions().add(origin, dest).width(10)
                    .color(getLineColor(settings.getSpouseColor())));
                polylines.add(line);
            }
        }
    }

    private void drawFamilyTreeLines(String eventID) {
        Event event = Events.getInstance().getEvent(eventID);
        int count = 0;

        locateFamily(event.getPersonID(), count);
    }

    private void locateFamily(String personID, int count) {
        LatLng origin;
        Event[] events;
        Person person = People.getInstance().getPerson(personID);
        if (count == 0) {
            Event first = Events.getInstance().getEvent(mClickedEventID);
            origin = new LatLng(first.getLatitude(), first.getLongitude());
        } else {
            events = Events.getInstance().getEvents(personID);
            origin = new LatLng(events[0].getLatitude(), events[0].getLongitude());
        }

        Person father = People.getInstance().getPerson(person.getFather());
        if (father != null) {
            Event[] fatherEvents = Events.getInstance().getEvents(father.getPersonID());
            LatLng dest = new LatLng(fatherEvents[0].getLatitude(), fatherEvents[0].getLongitude());
            int color = getLineColor(settings.getFamilyTreeColor());
            int width = 22 - 5 * count;

            Polyline fatherLine = mMap.addPolyline(new PolylineOptions().add(origin, dest).width(width).color(color));
            polylines.add(fatherLine);

            locateFamily(father.getPersonID(), count + 1);
        }

        Person mother = People.getInstance().getPerson(person.getMother());
        if (mother != null) {
            Event[] motherEvents = Events.getInstance().getEvents(mother.getPersonID());
            LatLng dest = new LatLng(motherEvents[0].getLatitude(), motherEvents[0].getLongitude());
            int color = getLineColor(settings.getFamilyTreeColor());
            int width = 22 - 5 * count;

            Polyline motherLine = mMap.addPolyline(new PolylineOptions().add(origin, dest).width(width).color(color));
            polylines.add(motherLine);

            locateFamily(mother.getPersonID(), count + 1);
        }
    }

    private void drawLifeStoryLines(String eventID) {
        Event event = Events.getInstance().getEvent(eventID);
        Person person = People.getInstance().getPerson(event.getPersonID());
        Event[] events = Events.getInstance().getEvents(person.getPersonID());

        PolylineOptions options = new PolylineOptions();
        for (Event e : events) {
            LatLng location = new LatLng(e.getLatitude(), e.getLongitude());
            options.add(location);
        }

        Polyline line = mMap.addPolyline(options.width(10).color(getLineColor(settings.getLifeStoryColor())));
        polylines.add(line);
    }

    public int getLineColor(int settingsValue) {
        switch (settingsValue) {
            case 0:
                return Color.GREEN;
            case 1:
                return Color.BLUE;
            case 2:
                return Color.RED;
            default:
                return Color.YELLOW;
        }
    }

    private void updateEventInfo(String eventID) {
        mClickedEventID = eventID;

        server.model.Event e = Events.getInstance().getEvent(eventID);
        server.model.Person p = People.getInstance().getPerson(e.getPersonID());

        String personName = p.getFirstName() + " " + p.getLastName();
        String eventType = e.getEventType();
        String place = e.getCity() + ", " + e.getCountry();
        String eventYear = "(" + e.getYear() + ")";
        String newText = personName + '\n' + eventType + ": " + place + " " + eventYear;

        mEventText.setText(newText);

        Drawable genderIcon = new IconDrawable(getActivity(), FontAwesomeIcons.fa_male).colorRes(R.color.male_icon).sizeDp(40);
        if (p.getGender().equals("f"))
            genderIcon = new IconDrawable(getActivity(), FontAwesomeIcons.fa_female).colorRes(R.color.female_icon).sizeDp(40);

        mEventImage.setImageDrawable(genderIcon);
    }

    private void newMarker(server.model.Event e) {
        String eventType = e.getEventType();

        if (!isFilteredOut(e)) {
            LatLng location = new LatLng(e.getLatitude(), e.getLongitude());
            String title = makeTitle(e.getPersonID(), eventType);
            float color;
            switch (eventType) {
                case "birth":
                    color = HUE_BLUE;
                    break;
                case "marriage":
                    color = HUE_GREEN;
                    break;
                case "death":
                    color = HUE_RED;
                    break;
                default:
                    color = HUE_YELLOW;
            }

            MarkerOptions options = new MarkerOptions().position(location).title(title).icon(defaultMarker(color));
            Marker marker = mMap.addMarker(options);
            marker.setTag(e.getEventID());
        }
    }

    private boolean isFilteredOut(Event e) {
        String eventType = e.getEventType();
        String matchEventType = eventType.substring(0, 1).toUpperCase() + eventType.substring(1) + " Events";
        String gender = People.getInstance().getPerson(e.getPersonID()).getGender();

        //Check for database events
        if (Filter.getInstance().valueOf(matchEventType)) {
            //Check for father's side
            if (Events.getInstance().isFathersSide(e)){
                if (Filter.getInstance().valueOf("Father's Side")) {
                    if (gender.equals("m")) {
                        return !Filter.getInstance().valueOf("Male Events");
                    } else {
                        return !Filter.getInstance().valueOf("Female Events");
                    }
                }
            } else if (Events.getInstance().isMothersSide(e)){ //Check mother's side
                if (Filter.getInstance().valueOf("Mother's Side")) {
                    if (gender.equals("m")) {
                        return !Filter.getInstance().valueOf("Male Events");
                    } else {
                        return !Filter.getInstance().valueOf("Female Events");
                    }
                }
            } else {
                if (gender.equals("m")) {
                    return !Filter.getInstance().valueOf("Male Events");
                } else {
                    return !Filter.getInstance().valueOf("Female Events");
                }
            }
        }

        return true;
    }

    private String makeTitle(String personID, String eventType) {
        server.model.Person p = People.getInstance().getPerson(personID);
        String personName = p.getFirstName() + " " + p.getLastName();
        return personName + "\'s " + eventType;
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        if (getArguments() == null) {
            super.onCreateOptionsMenu(menu, inflater);
            inflater.inflate(R.menu.menu_main, menu);
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.homeAsUp:
                NavUtils.navigateUpFromSameTask(getActivity());
                return true;
            case R.id.search:
                createNewSearchActivity();
                return true;
            case R.id.filter:
                createNewFilterActivity();
                return true;
            case R.id.settings:
                createNewSettingsActivity();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    public void createNewSearchActivity() {
        Intent intent = new Intent(getActivity(), SearchActivity.class);
        startActivity(intent);
    }

    public void createNewFilterActivity() {
        Intent intent = new Intent(getActivity(), FilterActivity.class);
        startActivity(intent);
    }

    public void createNewSettingsActivity() {
        Intent intent = new Intent(getActivity(), SettingsActivity.class);
        startActivity(intent);
    }

}

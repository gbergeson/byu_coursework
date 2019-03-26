package client.familymap;

import android.content.Intent;
import android.support.v4.app.NavUtils;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.joanzapata.iconify.Icon;
import com.joanzapata.iconify.widget.IconTextView;

import org.w3c.dom.Text;

import java.util.Arrays;
import java.util.List;

import server.model.Event;
import server.model.Events;
import server.model.People;
import server.model.Person;

public class PersonActivity extends AppCompatActivity {
    private String mPersonID;
    private RecyclerView mEventsRecyclerView;
    private RecyclerView mFamilyRecyclerView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_person);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        mPersonID = getIntent().getStringExtra("PersonID");
        Person person = People.getInstance().getPerson(mPersonID);

        ((TextView) findViewById(R.id.paFirstName)).setText(person.getFirstName());
        ((TextView) findViewById(R.id.paLastName)).setText(person.getLastName());

        String gender = "Female";
        if (person.getGender().equals("m"))
            gender = "Male";
        ((TextView) findViewById(R.id.paGender)).setText(gender);

        mEventsRecyclerView = (RecyclerView) findViewById(R.id.paEventsInfo);
        mEventsRecyclerView.setLayoutManager(new LinearLayoutManager(this));

        mFamilyRecyclerView = (RecyclerView) findViewById(R.id.paFamilyInfo);
        mFamilyRecyclerView.setLayoutManager(new LinearLayoutManager(this));

        updateUI();

        mEventsRecyclerView.setVisibility(View.GONE);
        mFamilyRecyclerView.setVisibility(View.GONE);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        NavUtils.navigateUpFromSameTask(this);
        return true;
    }

    private void updateUI() {
        List<Event> events = Arrays.asList(Events.getInstance().getEvents(mPersonID));
        List<Person> people = Arrays.asList(People.getInstance().getFamily(mPersonID));

        EventAdapter mEventAdapter = new EventAdapter(events);
        mEventsRecyclerView.setAdapter(mEventAdapter);

        PersonAdapter mPersonAdapter = new PersonAdapter(people);
        mFamilyRecyclerView.setAdapter(mPersonAdapter);
    }

    public void toggleEventView(View view) {
        mEventsRecyclerView.setVisibility(mEventsRecyclerView.isShown() ? View.GONE : View.VISIBLE);
    }

    public void toggleFamilyView(View view) {
        mFamilyRecyclerView.setVisibility(mFamilyRecyclerView.isShown() ? View.GONE : View.VISIBLE);
    }

    private class EventHolder extends RecyclerView.ViewHolder implements View.OnClickListener {
        private IconTextView mItemImage;
        private TextView mItemTitle;
        private TextView mItemDetails;
        private String mEventID;

        public EventHolder(LayoutInflater inflater, ViewGroup parent) {
            super(inflater.inflate(R.layout.list_item, parent, false));

            mItemImage = itemView.findViewById(R.id.listItemImage);
            mItemTitle = itemView.findViewById(R.id.listItemTitle);
            mItemDetails = itemView.findViewById(R.id.listItemDetails);

            itemView.setOnClickListener(this);
        }

        public void bind(Event event) {
            mEventID = event.getEventID();

            String eventImageCode = "{fa-map-marker}";
            mItemImage.setText(eventImageCode);

            String itemTitle = event.getEventType() + ": " + event.getCity() + ", " + event.getCountry() + " (" + event.getYear() + ")";
            mItemTitle.setText(itemTitle);
            String itemDetails = People.getInstance().getPerson(event.getPersonID()).getFullName();
            mItemDetails.setText(itemDetails);
        }

        @Override
        public void onClick(View view) {
            Intent intent = new Intent(PersonActivity.this, MapActivity.class);
            intent.putExtra("EventID", mEventID);
            startActivity(intent);
        }
    }

    private class EventAdapter extends RecyclerView.Adapter<EventHolder> {
        private List<Event> mEvents;

        public EventAdapter(List<Event> events) {
            mEvents = events;
        }

        @Override
        public EventHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            LayoutInflater layoutInflater = LayoutInflater.from(PersonActivity.this);

            return new EventHolder(layoutInflater, parent);
        }

        @Override
        public void onBindViewHolder(EventHolder holder, int position) {
            Event event = mEvents.get(position);
            holder.bind(event);
        }

        @Override
        public int getItemCount() {
            return mEvents.size();
        }
    }

    private class PersonHolder extends RecyclerView.ViewHolder implements View.OnClickListener {
        private IconTextView mItemImage;
        private TextView mItemTitle;
        private TextView mItemDetails;
        private String mEventPersonID;

        public PersonHolder(LayoutInflater inflater, ViewGroup parent) {
            super(inflater.inflate(R.layout.list_item, parent, false));

            mItemImage = itemView.findViewById(R.id.listItemImage);
            mItemTitle = itemView.findViewById(R.id.listItemTitle);
            mItemDetails = itemView.findViewById(R.id.listItemDetails);

            itemView.setOnClickListener(this);
        }

        public void bind(Person person) {
            String personImageCode = "{fa-female}";
            if (person.getGender().equals("m")) {
                personImageCode = "{fa-male}";
                mItemImage.setTextColor(getResources().getColor(R.color.male_icon));
            } else {
                mItemImage.setTextColor(getResources().getColor(R.color.female_icon));
            }
            mItemImage.setText(personImageCode);
            mItemTitle.setText(person.getFullName());
            mItemDetails.setText(person.getRelationship());
            mEventPersonID = person.getPersonID();
        }

        @Override
        public void onClick(View view) {
            Intent intent = new Intent(PersonActivity.this, PersonActivity.class);
            intent.putExtra("PersonID", mEventPersonID);
            startActivity(intent);
        }
    }

    private class PersonAdapter extends RecyclerView.Adapter<PersonHolder> {
        private List<Person> mPeople;

        public PersonAdapter(List<Person> people) {
            mPeople = people;
        }

        @Override
        public PersonHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            LayoutInflater layoutInflater = LayoutInflater.from(PersonActivity.this);

            return new PersonHolder(layoutInflater, parent);
        }

        @Override
        public void onBindViewHolder(PersonHolder holder, int position) {
            Person person = mPeople.get(position);
            holder.bind(person);
        }

        @Override
        public int getItemCount() {
            return mPeople.size();
        }
    }
}

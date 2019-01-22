package client.familymap;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;

import com.joanzapata.iconify.widget.IconTextView;

import java.util.ArrayList;
import java.util.List;

import server.model.Event;
import server.model.Events;
import server.model.People;
import server.model.Person;

public class SearchActivity extends AppCompatActivity {
    private RecyclerView mSearchResults;
    private EditText mSearchBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_search);

        mSearchResults = findViewById(R.id.searchResults);
        mSearchBar = findViewById(R.id.searchBar);
        
        mSearchResults.setLayoutManager(new LinearLayoutManager(this));

        mSearchBar.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView textView, int i, KeyEvent keyEvent) {
                updateUI();
                return true;
            }
        });
    }
    
    private void updateUI() {
        List<Person> people = People.getInstance().search(mSearchBar.getText().toString());
        List<Event> events = Events.getInstance().search(mSearchBar.getText().toString());
        ArrayList<Item> results = new ArrayList<>();

        for (Person p : people)
            results.add(new Item(p.getFullName(), p.getGender(), p.getPersonID()));

        for (Event e : events) {
            String itemTitle = e.getEventType() + ": " + e.getCity() + ", " + e.getCountry() + " (" + e.getYear() + ")";
            String itemDetails = People.getInstance().getPerson(e.getPersonID()).getFullName();
            String itemText = itemTitle + "\n" + itemDetails;

            results.add(new Item(itemText, e.getEventID()));
        }

        mSearchResults.setAdapter(new ItemAdapter(results));
    }

    private class ItemHolder extends RecyclerView.ViewHolder implements View.OnClickListener {
        private IconTextView mItemImage;
        private TextView mItemTitle;
        private String mItemID;
        private String mItemType = "Person";
        private String eventImageCode = "{fa-map-marker}";
        private String maleImageCode = "{fa-male}";
        private String femaleImageCode = "{fa-female}";

        public ItemHolder(LayoutInflater inflater, ViewGroup parent) {
            super(inflater.inflate(R.layout.list_item, parent, false));

            mItemImage = itemView.findViewById(R.id.listItemImage);
            mItemTitle = itemView.findViewById(R.id.listItemTitle);

            itemView.setOnClickListener(this);
        }

        public void bind(Item item) {
            mItemID = item.ID;
            mItemTitle.setText(item.text);

            if (item.gender == null) {
                mItemImage.setText(eventImageCode);
                mItemType = "Event";
                mItemImage.setTextColor(getResources().getColor(R.color.gray));
            } else if (item.gender.equals("m")) {
                mItemImage.setTextColor(getResources().getColor(R.color.male_icon));
                mItemImage.setText(maleImageCode);
            } else {
                mItemImage.setTextColor(getResources().getColor(R.color.female_icon));
                mItemImage.setText(femaleImageCode);
            }
        }

        @Override
        public void onClick(View view) {
            if (mItemType.equals("Event")) {
                Intent intent = new Intent(SearchActivity.this, MapActivity.class);
                intent.putExtra("EventID", mItemID);
                startActivity(intent);
            } else {
                Intent intent = new Intent(SearchActivity.this, PersonActivity.class);
                intent.putExtra("PersonID", mItemID);
                startActivity(intent);
            }
        }
    }

    private class ItemAdapter extends RecyclerView.Adapter<ItemHolder> {
        private List<Item> mItems;

        public ItemAdapter(List<Item> items) {
            mItems = items;
        }

        @Override
        public ItemHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            LayoutInflater layoutInflater = LayoutInflater.from(SearchActivity.this);

            return new ItemHolder(layoutInflater, parent);
        }

        @Override
        public void onBindViewHolder(ItemHolder holder, int position) {
            Item item = mItems.get(position);
            holder.bind(item);
        }

        @Override
        public int getItemCount() {
            return mItems.size();
        }
    }

    private class Item {
        String text, gender, ID;

        Item(String text, String gender, String ID) {
            this.text = text;
            this.gender = gender;
            this.ID = ID;
        }
        Item(String text, String ID) {
            this.text = text;
            this.gender = null;
            this.ID = ID;
        }
    }
}

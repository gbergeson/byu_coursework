package client.familymap;

import android.os.AsyncTask;
import android.support.v4.app.NavUtils;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CompoundButton;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import server.ProxyServer;
import server.model.Events;
import server.model.People;
import server.model.User;
import server.result.Result;

public class SettingsActivity extends AppCompatActivity {
    private Spinner lifeStorySelect, familyTreeSelect, spouseSelect, mapTypeSelect;
    private Settings settings = Settings.getInstance();
    private Switch lifeStorySwitch, familyTreeSwitch, spouseSwitch;
    private TextView reSyncButton, logoutButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        setFieldsAndValues();

        logoutButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                User.getInstance().destroy();
                Events.getInstance().destroy();
                People.getInstance().destroy();
                Settings.getInstance().destroy();
                Filter.getInstance().destroy();

                NavUtils.navigateUpFromSameTask(SettingsActivity.this);
            }
        });
        reSyncButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                SyncDataTask syncDataTask = new SyncDataTask();
                syncDataTask.execute();
            }
        });
        lifeStorySwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                settings.setLifeStoryOn(isChecked);
            }
        });
        familyTreeSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                settings.setFamilyTreeOn(isChecked);
            }
        });
        spouseSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                settings.setSpouseOn(isChecked);
            }
        });
        lifeStorySelect.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                settings.setLifeStoryColor(i);
            }
            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });
        familyTreeSelect.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                settings.setFamilyTreeColor(i);
            }
            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });
        spouseSelect.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                settings.setSpouseColor(i);
            }
            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });
        mapTypeSelect.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                settings.setMapType(i);
            }
            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });
    }

    private void setFieldsAndValues() {
        lifeStorySelect = findViewById(R.id.lifeStoryColors);
        familyTreeSelect = findViewById(R.id.familyTreeColors);
        spouseSelect = findViewById(R.id.spouseColors);
        mapTypeSelect = findViewById(R.id.mapType);

        setAdapters();

        lifeStorySelect.setSelection(settings.getLifeStoryColor());
        familyTreeSelect.setSelection(settings.getFamilyTreeColor());
        spouseSelect.setSelection(settings.getSpouseColor());
        mapTypeSelect.setSelection(settings.getMapType());

        lifeStorySwitch = findViewById(R.id.lifeStorySwitch);
        familyTreeSwitch = findViewById(R.id.familyTreeSwitch);
        spouseSwitch = findViewById(R.id.spouseSwitch);

        lifeStorySwitch.setChecked(settings.isLifeStoryOn());
        familyTreeSwitch.setChecked(settings.isFamilyTreeOn());
        spouseSwitch.setChecked(settings.isSpouseOn());

        reSyncButton = findViewById(R.id.resyncButton);
        logoutButton = findViewById(R.id.logoutButton);
    }

    private void setAdapters() {
        setColorAdapter(lifeStorySelect);
        setColorAdapter(familyTreeSelect);
        setColorAdapter(spouseSelect);

        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this,
                R.array.map_type, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mapTypeSelect.setAdapter(adapter);
    }

    private void setColorAdapter(Spinner colorSpinner) {
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this,
                R.array.color_select, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        colorSpinner.setAdapter(adapter);
    }

    @Override
    public void onBackPressed() {
        NavUtils.navigateUpFromSameTask(this);
    }

    public class SyncDataTask extends AsyncTask<Void, Void, Result> {
        @Override
        protected server.result.Result doInBackground(Void... params) {
            try {
                return ProxyServer.getInstance().retrieveUserFamilyData();
            } catch (Exception e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(server.result.Result r) {
            if (r instanceof server.result.Error) {
                server.result.Error res = (server.result.Error) r;
                displayToast(res.message);
            } else {
                Events.getInstance().normalizeEventTypes();
                server.model.Person res = People.getInstance().getPerson(User.getInstance().getPersonId());
                displayToast("Data re-synced for: " + res.getFirstName() + " " + res.getLastName());
                onBackPressed();
            }
        }
    }

    public void displayToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }
}

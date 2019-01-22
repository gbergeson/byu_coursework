package client.familymap;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MapActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map);

        FragmentManager fm = getSupportFragmentManager();
        Fragment map = new FMCMapFragment();

        map.setArguments(getIntent().getExtras());

        fm.beginTransaction().add(R.id.fragmentContainer, map).commit();
    }
}

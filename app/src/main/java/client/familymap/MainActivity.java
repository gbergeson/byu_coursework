package client.familymap;

import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;

import server.model.User;

public class MainActivity extends AppCompatActivity {
    private User user = User.getInstance();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        FragmentManager fm = getSupportFragmentManager();
        Fragment main;
        if (user.isLoggedIn())
            main = new FMCMapFragment();
        else
            main = new LoginFragment();

        fm.beginTransaction().add(R.id.mainContainer, main).commit();
    }

    public void onLogin() {
        user.setLoggedIn(true);
        FMCMapFragment mMapFragment = new FMCMapFragment();
        FragmentManager fm = getSupportFragmentManager();
        fm.beginTransaction().replace(R.id.mainContainer, mMapFragment).commit();
    }
}

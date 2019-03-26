package client.familymap;

import android.app.Activity;
import android.content.Context;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Toast;

import server.ProxyServer;
import server.model.Events;
import server.model.People;
import server.model.Person;
import server.model.User;
import server.result.Login;

public class LoginFragment extends Fragment {

    public LoginFragment() {
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
        return inflater.inflate(R.layout.fragment_login, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        final View mFragView = getView();

        Button mRegisterButton = (Button) mFragView.findViewById(R.id.registerButton);
        mRegisterButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                displayToast("Signing you up... Please wait.");
                try {
                    EditText e = (EditText)mFragView.findViewById(R.id.loginServerHost);
                    String serverHost = e.getText().toString();
                    e = (EditText)mFragView.findViewById(R.id.loginServerPort);
                    String serverPort = e.getText().toString();

                    ProxyServer.getInstance().setServer(serverHost, serverPort);

                    e = (EditText)mFragView.findViewById(R.id.loginUserName);
                    String username = e.getText().toString();
                    e = (EditText) mFragView.findViewById(R.id.loginPassword);
                    String password = e.getText().toString();
                    e = (EditText) mFragView.findViewById(R.id.loginFirstName);
                    String firstname = e.getText().toString();
                    e = (EditText) mFragView.findViewById(R.id.loginLastName);
                    String lastname = e.getText().toString();
                    e = (EditText) mFragView.findViewById(R.id.loginEmail);
                    String email = e.getText().toString();

                    String gender="";
                    RadioButton rb = (RadioButton) mFragView.findViewById(R.id.maleRadio);
                    if (rb.isChecked())
                        gender = "m";
                    else
                        gender = "f";

                    server.request.Register r = new server.request.Register(username, password, email, firstname, lastname, gender);

                    RegisterTask task = new RegisterTask();
                    task.execute(r);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });

        Button mLoginButton = (Button) mFragView.findViewById(R.id.loginButton);
        mLoginButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                displayToast("Logging you in...");
                try {
                    EditText e = (EditText)mFragView.findViewById(R.id.loginServerHost);
                    String serverHost = e.getText().toString();
                    e = (EditText)mFragView.findViewById(R.id.loginServerPort);
                    String serverPort = e.getText().toString();
                    ProxyServer.getInstance().setServer(serverHost, serverPort);

                    e = (EditText)mFragView.findViewById(R.id.loginUserName);
                    String username = e.getText().toString();
                    e = (EditText) mFragView.findViewById(R.id.loginPassword);
                    String password = e.getText().toString();
                    server.request.Login r = new server.request.Login(username, password);

                    LoginTask task = new LoginTask();
                    task.execute(r);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public class RegisterTask extends AsyncTask<server.request.Register, Void, server.result.Result> {
        @Override
        protected server.result.Result doInBackground(server.request.Register... r) {
            try {
                return ProxyServer.getInstance().registerUser(r[0]);
            } catch (Exception e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(server.result.Result r) {
            loginRegisterPostExecute(r);
        }
    }

    public class LoginTask extends AsyncTask<server.request.Login, Void, server.result.Result> {
            @Override
            protected server.result.Result doInBackground(server.request.Login... r) {
                try {
                    return ProxyServer.getInstance().loginUser(r[0]);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                return null;
            }

            @Override
            protected void onPostExecute(server.result.Result r) {
                loginRegisterPostExecute(r);
            }
    }

    private void loginRegisterPostExecute (server.result.Result r) {
        if(r == null) {
            displayToast("Internal Server Error.");
        } else if(r instanceof server.result.Error) {
            server.result.Error res = (server.result.Error) r;
            displayToast(res.message);
        } else if(r instanceof server.result.Login) {
            displayToast("Now syncing your data...");
            server.model.User.getInstance().setAuthToken(((Login) r).authToken);
            server.model.User.getInstance().setPersonId(((Login) r).personId);
            server.model.User.getInstance().setUserName(((Login) r).userName);

            SyncDataTask getPerson = new SyncDataTask();
            getPerson.execute();
        }
    }

    public class SyncDataTask extends AsyncTask<Void, Void, server.result.Result> {
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
                displayToast("Welcome, " + res.getFirstName() + " " + res.getLastName());
            }
            Activity parent = (MainActivity) getContext();
            ((MainActivity) parent).onLogin();
        }
    }

    private void displayToast(String s) {
        Toast.makeText(getActivity(), s, Toast.LENGTH_SHORT).show();
    }
}

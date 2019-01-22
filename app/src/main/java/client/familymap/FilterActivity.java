package client.familymap;

import android.support.v4.app.NavUtils;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;

import java.util.List;

public class FilterActivity extends AppCompatActivity {
    private RecyclerView mFiltersView;
    private FilterAdapter mFilterAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_filter);

        mFiltersView = findViewById(R.id.filterItemsView);
        mFiltersView.setLayoutManager(new LinearLayoutManager(this));

        updateUI();
    }

    private void updateUI() {
        List<String> eventTypes = Filter.getInstance().getKeys();

        mFilterAdapter = new FilterAdapter(eventTypes);
        mFiltersView.setAdapter(mFilterAdapter);
    }

    @Override
    public void onBackPressed() {
        NavUtils.navigateUpFromSameTask(this);
    }

    private class FilterHolder extends RecyclerView.ViewHolder {
        private TextView mFilterTitle;
        private Switch mSwitch;

        FilterHolder(LayoutInflater inflater, ViewGroup parent) {
            super(inflater.inflate(R.layout.filter_item, parent, false));

            mFilterTitle = itemView.findViewById(R.id.filterTitle);
            mSwitch = itemView.findViewById(R.id.filterSwitch);
        }

        public void bind(String title) {
            mFilterTitle.setText(title);
            mSwitch.setChecked(Filter.getInstance().valueOf(title));

            mSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                    Filter.getInstance().update((String)mFilterTitle.getText(), isChecked);
                }
            });
        }


    }

    private class FilterAdapter extends RecyclerView.Adapter<FilterHolder> {
        private List<String> mFilters;

        FilterAdapter(List<String> filters) {
            mFilters = filters;
        }

        @Override
        public FilterHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            LayoutInflater layoutInflater = LayoutInflater.from(FilterActivity.this);

            return new FilterHolder(layoutInflater, parent);
        }

        @Override
        public void onBindViewHolder(FilterHolder holder, int position) {
            String title = mFilters.get(position);
            holder.bind(title);
        }

        @Override
        public int getItemCount() {
            return mFilters.size();
        }
    }
}

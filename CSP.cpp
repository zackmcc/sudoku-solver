//
//  CSP.cpp
//  Sudoku
//
//  Created by cpsfaculty on 02/10/18.
//  Copyright (c) 2018 ___Ju Shen___. All rights reserved.
//

#include <stdio.h>

#include "CSP.h"

/************************************** Below are the three functions you need to implement ***************************************/

/*Check whether current state satisfy the constraints*/
bool CSP::goalCheck(const State state)
{
	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			for (int i = 0; i < 9; i++)
			{
				//variables[row][col].assignement = state.values[row][col];
				if (state.values[row][col] == 0)
					return false;
				//Check rows
				if (state.values[row][col] == state.values[row][i] && i != col)
					return false;
				//Check columns
				if (state.values[row][col] == state.values[i][col] && i != row)
					return false;
				
			}
			//Check sub-grid
			int sub_row_start = (row - (row % 3));
			int sub_col_start = (col - (col % 3));
			for (int sub_row = sub_row_start; sub_row < sub_row_start + 3; sub_row++)
			{
				for (int sub_col = sub_col_start; sub_col < sub_col_start + 3; sub_col++)
				{
					if (!(sub_row == row && sub_col == col))
					{
						if (state.values[row][col] == state.values[sub_row][sub_col])
							return false;
					}

				}
			}

		}
	}

    return true;
}



/*Update Domain for the forward checking*/
void CSP::updateDomain(const State state)
{
	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			//check if updated from a previous value
			bool reassigned = false;
			if (variables[row][col].previous_assignment != variables[row][col].assignement && variables[row][col].initialized)
				reassigned = true;
			/*If the value has not yet been initalized and the current value is not 0 then 
			  the cell has been initialized*/
			if (!variables[row][col].initialized && state.values[row][col] != 0)
				variables[row][col].initialized = true;

			// Check if cell has a value
			if (state.values[row][col] != 0 || variables[row][col].initialized)
			{
				for (int i = 0; i < 9; i++)
				{
					// add value back to col and row domain if cell was reset to zero
					if (reassigned && variables[row][col].previous_assignment != 0)
					{
						//If the value is not found in the domain then it needs to be added back in
						if (std::find(variables[i][col].domain.begin(), variables[i][col].domain.end(), variables[row][col].previous_assignment) == variables[i][col].domain.end())
							variables[i][col].domain.push_back(variables[row][col].previous_assignment);
						if (std::find(variables[row][i].domain.begin(), variables[row][i].domain.end(), variables[row][col].previous_assignment) == variables[row][i].domain.end())
							variables[row][i].domain.push_back(variables[row][col].previous_assignment);
					}

					//Check if comain value in this column contains the current state value, if it does it needs to be removed
					if (std::find(variables[i][col].domain.begin(), variables[i][col].domain.end(), state.values[row][col]) != variables[i][col].domain.end())
					{
						variables[i][col].domain.erase(std::remove(variables[i][col].domain.begin(), variables[i][col].domain.end(), state.values[row][col]), variables[i][col].domain.end());
						//sort to put domain in numerical order, this makes it easier to read but breaks the random value setting, I left it here to assist debugging
						//std::sort(variables[i][col].domain.begin(), variables[i][col].domain.end());//sort to put domain in numerical order (not necessary just makes it easier to read
					}
					//Check if comain value in this row contains the current state value, if it does it needs to be removed
					if (std::find(variables[row][i].domain.begin(), variables[row][i].domain.end(), state.values[row][col]) != variables[row][i].domain.end())
					{
						//Remove assigned value from domain
						variables[row][i].domain.erase(std::remove(variables[row][i].domain.begin(), variables[row][i].domain.end(), state.values[row][col]),  variables[row][i].domain.end());
						//sort to put domain in numerical order, this makes it easier to read but breaks the random value setting, I left it here to assist debugging
						//std::sort(variables[row][i].domain.begin(), variables[row][i].domain.end());//sort domain so it is in numerical order
					}
				
					//Iterate through sub_box rows
					int sub_row_start = (row - (row % 3));
					int sub_col_start = (col - (col % 3));
					for (int sub_row = sub_row_start; sub_row < sub_row_start + 3; sub_row++)
					{
						// Iterate through sub_box columns
						for (int sub_col = sub_col_start; sub_col < sub_col_start + 3; sub_col++)
						{
							//Ensure hat we are not modifying the current cell
							if (!(sub_row == row && sub_col == col))
							{
								/*This checks to see if the value has been reassigned from a previous value, and if the 
								  current value already exists in this sub cell's domain */
								if (reassigned && variables[row][col].previous_assignment != 0
									&& std::find(variables[sub_row][sub_col].domain.begin(), 
												 variables[sub_row][sub_col].domain.end(), 
												 variables[row][col].previous_assignment) == variables[sub_row][sub_col].domain.end())
								{
									variables[sub_row][sub_col].domain.push_back(variables[row][col].previous_assignment);
								}
								//If the current value is found in the domain then it should be removed	
								if (std::find(variables[sub_row][sub_col].domain.begin(), 
									          variables[sub_row][sub_col].domain.end(), 
											  state.values[row][col]) != variables[sub_row][sub_col].domain.end())
								{
									variables[sub_row][sub_col].domain.erase(std::remove(variables[sub_row][sub_col].domain.begin(), variables[sub_row][sub_col].domain.end(), state.values[row][col]), variables[sub_row][sub_col].domain.end());
									//sort to put domain in numerical order, this makes it easier to read but breaks the random value setting, I left it here to assist debugging
									//std::sort(variables[sub_row][sub_col].domain.begin(), variables[sub_row][sub_col].domain.end());
								}

							}

						}//end for sub_col

					} //end for sub_row
					

				}
				
			}
			//Update previous assignment
			variables[row][col].previous_assignment = state.values[row][col];
		}

	}
}



/*Arc consistency use*/
void CSP::arcConsistency(const State state)
{

}


/************************************************	End of Assignment ***********************************************/




CSP::CSP()
{
    /*Initially assign the domain, assignment for each variable and initialize the current state*/
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            variables[y][x].assignement = 0; //Initialize the assignment
            
            /*Initialize the domain*/
            for(int i = 1; i <= 9; i++)
            {
                variables[y][x].domain.push_back(i);
            }
            
            cur_state.values[y][x] = 0; //Initizlize the current state
            
        }
    }
    
    alg_opt = 1; //initially set it as back track
    
    srand(time(NULL));
    random = 0;
}


CSP::~CSP()
{
    
}




void CSP::setData(int *data)
{
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            int idx = y * 9 + x;
            variables[y][x].assignement = data[idx]; //Initialize the assignment
            cur_state.values[y][x] = data[idx]; //Initizlize the current state
            
        }
    }
}

void CSP::clearData()
{
    /*Initially assign the domain, assignment for each variable and initialize the current state*/
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            variables[y][x].assignement = 0; //Initialize the assignment
            
            /*Initialize the domain*/
            variables[y][x].domain.clear();
            for(int i = 1; i <= 9; i++)
            {
                variables[y][x].domain.push_back(i);
            }
            
            cur_state.values[y][x] = 0; //Initizlize the current state
            
        }
    }
    
    /*Check whether a random domain is use*/
    if(random == 1)
        reshuffleDomain();
    
    repeating_list.clear();
    while(!assigned_variables.empty())
    {
        assigned_variables.pop();
        repeating_list.clear();
    }
    
}


void CSP::reshuffleDomain()
{
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        std::random_shuffle( variables[y][x].domain.begin(), variables[y][x].domain.end() );
    }
}

void CSP::sortDomain()
{
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        std::sort( variables[y][x].domain.begin(), variables[y][x].domain.end() );
    }
}

/*Cancel last assignment*/
int CSP::goBack(int *chosen_cell)
{
    if(assigned_variables.size() > 0)
    {
        int cur_id = assigned_variables.top(); /*Remove last options*/
        assigned_variables.pop(); //pop out last option
        int y = cur_id / 9;
        int x = cur_id % 9;
        
        variables[y][x].assignement = 0; //assign the cell to zero
        cur_state.values[y][x] = 0; //update the assignment
        *chosen_cell = cur_id;
        
       // printf("(%d, %d)\n", y, x);
        if(alg_opt == 2)
        {
            updateDomain(cur_state);
        }
        else if (alg_opt == 3)
        {
            arcConsistency(cur_state);
        }
        
    }
    
    return goalCheck(cur_state);
    
}


bool CSP::arcCheckingOrder(int *chosen_cell)
{
    arcConsistency(cur_state);
    
    
    
    /*First go through all the variables and do backtrack if there is no empty domain */
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() == 0)
        {
            int available_assignemnt = 0; //an indicatior whether there are possible possible varaibles to be re-assigned
            while (available_assignemnt == 0) {
                int cur_id = assigned_variables.top();
                int y = cur_id / 9;
                int x = cur_id % 9;
                variables[y][x].assignement = 0;
                cur_state.values[y][x] = 0;
                arcConsistency(cur_state);
                
                
                for(int i = 0; i < variables[y][x].domain.size(); i++)
                {
                    State temp_state;
                    temp_state = cur_state;
                    temp_state.values[y][x] = variables[y][x].domain[i];
                    if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
                    {
                        cur_state = temp_state;
                        variables[y][x].assignement = variables[y][x].domain[i];
                        repeating_list.push_back(temp_state);
                        available_assignemnt = 1;
                        *chosen_cell = cur_id;
                        arcConsistency(cur_state);
                        return false; //get out of the current varaible assignment
                    }
                }
                
                if(available_assignemnt == 0) //if all the domain values have been tried for current variable
                {
                    variables[y][x].assignement = 0;
                    cur_state.values[y][x] = 0;
                    assigned_variables.pop();
                    
                }
            }
            
        }
        
    }
    
    /*If there is no variable that has empty domain, then assign variable here*/
    /*First go through all the variables and do backtrack if there is no empty domain */
    int count = 0;
    while (count < 81)
    {
        /*Find the index of minimum number of domain*/
        int min_idx = 0;
        int min_num = 10; //because the maximum number of domain is 10
        for(int i = 0; i < 81; i++)
        {
            int y = i / 9;
            int x = i % 9;
            if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
            {
                if (variables[y][x].domain.size() < min_num) {
                    min_idx = i;
                    min_num = variables[y][x].domain.size();
                }
            }
        }
        
        int y = min_idx / 9;
        int x = min_idx % 9;
        
        /*If there is any varable has not been assigned yet, assign it and return it*/
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
        {
            /*Find the smalles number in domain to assign it. Here no update domain for bracktrack*/
            int id_min = 0;
            cur_state.values[y][x] = variables[y][x].domain[id_min];
            variables[y][x].assignement = variables[y][x].domain[id_min];
            assigned_variables.push(min_idx); //push the variable into stack, which will be used for backtrack (or DFS)
            repeating_list.push_back(cur_state); //make this state into the repeat_list
            *chosen_cell = 9 * y + x;
            
            arcConsistency(cur_state); //Every time modify the assignment update the domain
            
            return false;
        }
        
        count++;
        
    }
    
    if(goalCheck(cur_state))
    {
        printf("find the goal\n");
        return true;
    }
	else
	{
		int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
		while (available_assignemnt == 0) {
			int cur_id = assigned_variables.top();
			int y = cur_id / 9;
			int x = cur_id % 9;
			variables[y][x].assignement = 0;
			cur_state.values[y][x] = 0;
			arcConsistency(cur_state);
			for(int i = 0; i < variables[y][x].domain.size(); i++)
			{
				State temp_state;
				temp_state = cur_state;
				temp_state.values[y][x] = variables[y][x].domain[i];
				if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
				{
					cur_state = temp_state;
					variables[y][x].assignement = variables[y][x].domain[i];
					repeating_list.push_back(cur_state);
					available_assignemnt = 1;
					*chosen_cell = cur_id;
					break; //get out of the current varaible assignment
				}
			}

			if(available_assignemnt == 0) //if all the domain values have been tried for current variable
			{

				assigned_variables.pop();

			}
		}

		return false;
	}
    return false;
    
}



/*arcChecking without ordering*/
bool CSP::arcChecking(int *chosen_cell)
{
    arcConsistency(cur_state);
    
    
    
    /*First go through all the variables and do backtrack if there is no empty domain */
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() == 0)
        {
            int available_assignemnt = 0; //an indicatior whether there are possible possible varaibles to be re-assigned
            while (available_assignemnt == 0) {
                int cur_id = assigned_variables.top();
                int y = cur_id / 9;
                int x = cur_id % 9;
                variables[y][x].assignement = 0;
                cur_state.values[y][x] = 0;
                arcConsistency(cur_state);
                
                
                for(int i = 0; i < variables[y][x].domain.size(); i++)
                {
                    State temp_state;
                    temp_state = cur_state;
                    temp_state.values[y][x] = variables[y][x].domain[i];
                    if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
                    {
                        cur_state = temp_state;
                        variables[y][x].assignement = variables[y][x].domain[i];
                        repeating_list.push_back(temp_state);
                        available_assignemnt = 1;
                        *chosen_cell = cur_id;
                        arcConsistency(cur_state);
                        return false; //get out of the current varaible assignment
                    }
                }
                
                if(available_assignemnt == 0) //if all the domain values have been tried for current variable
                {
                    variables[y][x].assignement = 0;
                    cur_state.values[y][x] = 0;
                    assigned_variables.pop();
                    
                }
            }
            
        }
    }
    
    /*If there is no variable that has empty domain, then assign variable here*/
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        /*If there is any varable has not been assigned yet, assign it and return it*/
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
        {
            /*Find the smalles number in domain to assign it. Here no update domain for bracktrack*/
            int id_min = 0;
            cur_state.values[y][x] = variables[y][x].domain[id_min];
            variables[y][x].assignement = variables[y][x].domain[id_min];
            assigned_variables.push(i); //push the variable into stack, which will be used for backtrack (or DFS)
            repeating_list.push_back(cur_state); //make this state into the repeat_list
            *chosen_cell = 9 * y + x;
            
            arcConsistency(cur_state); //Every time modify the assignment update the domain
            
            return false;
        }
        
    }
    
    if(goalCheck(cur_state))
    {
        printf("find the goal\n");
        return true;
    }
	else
	{
		int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
		while (available_assignemnt == 0) {
			int cur_id = assigned_variables.top();
			int y = cur_id / 9;
			int x = cur_id % 9;
			variables[y][x].assignement = 0;
			cur_state.values[y][x] = 0;
			arcConsistency(cur_state);
			for(int i = 0; i < variables[y][x].domain.size(); i++)
			{
				State temp_state;
				temp_state = cur_state;
				temp_state.values[y][x] = variables[y][x].domain[i];
				if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
				{
					cur_state = temp_state;
					variables[y][x].assignement = variables[y][x].domain[i];
					repeating_list.push_back(cur_state);
					available_assignemnt = 1;
					*chosen_cell = cur_id;
					break; //get out of the current varaible assignment
				}
			}

			if(available_assignemnt == 0) //if all the domain values have been tried for current variable
			{

				assigned_variables.pop();

			}
		}

		return false;
	}
    return false;

}



/*Forward Checking algorithm*/
bool CSP::forwardChecking(int *chosen_cell)
{
    updateDomain(cur_state); //the first step is based on current setting to update the domain
    
    
    
    /*First go through all the variables and do backtrack whether there is an empty domain */
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() == 0)
        {
            int available_assignemnt = 0; //an indicatior whether there are possible possible varaibles to be re-assigned
            while (available_assignemnt == 0) {
                int cur_id = assigned_variables.top();
                int y = cur_id / 9;
                int x = cur_id % 9;
                variables[y][x].assignement = 0;
                cur_state.values[y][x] = 0;
                updateDomain(cur_state);
                for(int i = 0; i < variables[y][x].domain.size(); i++)
                {
                    State temp_state;
                    temp_state = cur_state;
                    temp_state.values[y][x] = variables[y][x].domain[i];
                    if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
                    {
                        cur_state = temp_state;
                        variables[y][x].assignement = variables[y][x].domain[i];
                        repeating_list.push_back(temp_state);
                        available_assignemnt = 1;
                        *chosen_cell = cur_id;
                        updateDomain(cur_state);
                        return false; //get out of the current varaible assignment
                    }
                }
                
                if(available_assignemnt == 0) //if all the domain values have been tried for current variable
                {
                    variables[y][x].assignement = 0;
                    cur_state.values[y][x] = 0;
                    assigned_variables.pop();
                    
                }
            }
            
        }
    }
    
    /*If there is no variable that has empty domain, then assign variable here*/
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        /*If there is any varable has not been assigned yet, assign it and return it*/
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
        {
            /*Find the smalles number in domain to assign it. Here no update domain for bracktrack*/
            int id_min = 0;
            cur_state.values[y][x] = variables[y][x].domain[id_min];
            variables[y][x].assignement = variables[y][x].domain[id_min];
            assigned_variables.push(i); //push the variable into stack, which will be used for backtrack (or DFS)
            repeating_list.push_back(cur_state); //make this state into the repeat_list
            *chosen_cell = 9 * y + x;
            
            updateDomain(cur_state); //Every time modify the assignment update the domain
            
            return false;
        }

    }
    
    if(goalCheck(cur_state))
    {
        printf("find the goal\n");
        return true;
    }else
	{
		int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
		while (available_assignemnt == 0) {
			int cur_id = assigned_variables.top();
			int y = cur_id / 9;
			int x = cur_id % 9;
			variables[y][x].assignement = 0;
			cur_state.values[y][x] = 0;
			updateDomain(cur_state);
			for(int i = 0; i < variables[y][x].domain.size(); i++)
			{
				State temp_state;
				temp_state = cur_state;
				temp_state.values[y][x] = variables[y][x].domain[i];
				if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
				{
					cur_state = temp_state;
					variables[y][x].assignement = variables[y][x].domain[i];
					repeating_list.push_back(cur_state);
					available_assignemnt = 1;
					*chosen_cell = cur_id;
					break; //get out of the current varaible assignment
				}
			}

			if(available_assignemnt == 0) //if all the domain values have been tried for current variable
			{

				assigned_variables.pop();

			}
		}

		return false;
	}
    return false;
    
}


/*Forward Checking algorithm*/
bool CSP::forwardCheckingOrder(int *chosen_cell)
{

    updateDomain(cur_state); //the first step is based on current setting to update the domain
    
    
    
    /*First go through all the variables and do backtrack whether there is an empty domain */
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() == 0)
        {
            int available_assignemnt = 0; //an indicatior whether there are possible possible varaibles to be re-assigned
            while (available_assignemnt == 0) {
                int cur_id = assigned_variables.top();
                int y = cur_id / 9;
                int x = cur_id % 9;
                variables[y][x].assignement = 0;
                cur_state.values[y][x] = 0;
                updateDomain(cur_state);
                for(int i = 0; i < variables[y][x].domain.size(); i++)
                {
                    State temp_state;
                    temp_state = cur_state;
                    temp_state.values[y][x] = variables[y][x].domain[i];
                    if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
                    {
                        cur_state = temp_state;
                        variables[y][x].assignement = variables[y][x].domain[i];
                        repeating_list.push_back(temp_state);
                        available_assignemnt = 1;
                        *chosen_cell = cur_id;
                        updateDomain(cur_state);
                        return false; //get out of the current varaible assignment
                    }
                }
                
                if(available_assignemnt == 0) //if all the domain values have been tried for current variable
                {
                    variables[y][x].assignement = 0;
                    cur_state.values[y][x] = 0;
                    assigned_variables.pop();
                    
                }
            }
            
        }
    }
    
    
    int count = 0;
    while (count < 81)
    {
        /*Find the index of minimum number of domain*/
        int min_idx = 0;
        int min_num = 10; //because the maximum number of domain is 10
        for(int i = 0; i < 81; i++)
        {
            int y = i / 9;
            int x = i % 9;
            if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
            {
                if (variables[y][x].domain.size() < min_num) {
                    min_idx = i;
                    min_num = variables[y][x].domain.size();
                }
            }
        }
        
        int y = min_idx / 9;
        int x = min_idx % 9;
        
        /*If there is any varable has not been assigned yet, assign it and return it*/
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
        {
            /*Find the smalles number in domain to assign it. Here no update domain for bracktrack*/
            int id_min = 0;
            cur_state.values[y][x] = variables[y][x].domain[id_min];
            variables[y][x].assignement = variables[y][x].domain[id_min];
            assigned_variables.push(min_idx); //push the variable into stack, which will be used for backtrack (or DFS)
            repeating_list.push_back(cur_state); //make this state into the repeat_list
            *chosen_cell = 9 * y + x;
            
            updateDomain(cur_state); //Every time modify the assignment update the domain
            
            return false;
        }
        
        count++;
    }
    
    if(goalCheck(cur_state))
    {
        printf("find the goal\n");
        return true;
    }
	else
	{
		int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
		while (available_assignemnt == 0) {
			int cur_id = assigned_variables.top();
			int y = cur_id / 9;
			int x = cur_id % 9;
			variables[y][x].assignement = 0;
			cur_state.values[y][x] = 0;
			updateDomain(cur_state);
			for(int i = 0; i < variables[y][x].domain.size(); i++)
			{
				State temp_state;
				temp_state = cur_state;
				temp_state.values[y][x] = variables[y][x].domain[i];
				if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
				{
					cur_state = temp_state;
					variables[y][x].assignement = variables[y][x].domain[i];
					repeating_list.push_back(cur_state);
					available_assignemnt = 1;
					*chosen_cell = cur_id;
					break; //get out of the current varaible assignment
				}
			}

			if(available_assignemnt == 0) //if all the domain values have been tried for current variable
			{

				assigned_variables.pop();

			}
		}

		return false;
	}

    return false;
    
}



/*Back Track to solve the proble*/
bool CSP::backTrack(int *chosen_cell)
{
    
  
    
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        
        /*If there is any varable has not been assigned yet, assign it and break*/
        if(cur_state.values[y][x] == 0)
        {
            
            /*Find the smalles number in domain to assign it. Here no update domain for bracktrack*/
            int id_min = 0;
            cur_state.values[y][x] = variables[y][x].domain[id_min];
            variables[y][x].assignement = variables[y][x].domain[id_min];
            assigned_variables.push(i); //push the variable into stack, which will be used for backtrack (or DFS)
            repeating_list.push_back(cur_state); //make this state into the repeat_list
            *chosen_cell = 9 * y + x;
            return false;
            
        }
    }
    
    /*If all the the variable are assigned*/
    {
        if(assigned_variables.size() == 0)//reset all the variables if there are no any varaibles assigned yet
        {
            for(int i = 0; i < 81; i++)
            {
                assigned_variables.push(i);
            }
        }
        
        if(goalCheck(cur_state))
        {
            printf("find the goal\n");
            return true;
        }
        else
        {
            int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
            while (available_assignemnt == 0) {
                int cur_id = assigned_variables.top();
                int y = cur_id / 9;
                int x = cur_id % 9;
                
                
                for(int i = 0; i < variables[y][x].domain.size(); i++)
                {
                    State temp_state;
                    temp_state = cur_state;
                    temp_state.values[y][x] = variables[y][x].domain[i];
                    if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
                    {
                        cur_state = temp_state;
                        variables[y][x].assignement = variables[y][x].domain[i];
                        repeating_list.push_back(cur_state);
                        available_assignemnt = 1;
                        *chosen_cell = cur_id;
                        break; //get out of the current varaible assignment
                    }
                }
                
                if(available_assignemnt == 0) //if all the domain values have been tried for current variable
                {
                    variables[y][x].assignement = 0;
                    cur_state.values[y][x] = 0;
                    assigned_variables.pop();
                    
                }
            }
            
            return false;
        }
    }
}
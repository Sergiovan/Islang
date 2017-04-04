#This is a comment
/* This is a comment too */

import colors   #Will search for "colors.isl"
import "fruits.isl" #Filenames as strings also valid
# import "/path/to/some/import/import.isl" #Interpreter should be able to find this and import it no problemo
import stuff as other_stuff #If you don't like the original name, just change it


/*Anything imported right now is in the global scope, for everyone to access*/
bool cube_solved    -> false #Some random boolean
num  deaths         -> 0     #All numbers stored as floats
str  fate           -> "A certain death" #Yay, strings
str  hope           -> "Your hope right now is {deaths}" #Strings may contain values in them, which will be converted to string format. That'll show as "Your hope right now is 0", unless the value of `deaths` changes
list ccolor         -> ["Red", "Gr{"Hello"}een", "Bl{"It's a {cube_solved}"}ue"] #Lists can only be of one type. Mixed lists are an error
enum states         -> {ALIVE, DEAD}           #Enumerators for convenient values
val  state          -> states.DEAD             #Actually just an integer, but don't tell anyone. It's a secret

list list_list -> [[1,2,3,4], [5,6,7,8]] #Lists of lists will be checked rigurously. Don't be mixing types, please
num list_list_num -> list_list[0][0]     #Indexing!

/*Locations represent locations in your created place. They can also be used for scoping*/
location island: #Global scope may only contain one location
    list amounts -> [1 ~ 90%, 2 ~ 10%] #Lists can add a probability to each of its elements. Careful though, the existance or not of probability is part of the list type. Also they must add up to 1
    num amount_to_pick -> amounts* #* is the pick operator. It choses X elements out of a list, based on stored probabilities. In case there are no stored probabilities, it'll chose at random. In this case since we provided no number, it'll pick just one element 

    location floating_island: #Locations within locations. Will be island.floating_island from the global scope
        appear 100%                 #Locations might not always appear inside other locations, so you can give them a chance-based value. By default they'll always appear
        color  ::dark_green         #Color for the console prompt. Colors are stored in the special scope, accessed with ::
        name   "Floating island"    #The name of the location to show on the console, if none is provided, it'll just use the name you gave them in the program

        location chest (100%): #Becomes island.floating_island.chest in global scope. Shortcut for appear 100%
            #appear 75%  #This would be an error

            enum locks     -> {KEY, BUTTONS}
            val  lock      -> locks{90%, 10%}* #This construct creates a list from an enum, and then picks one
            bool is_mimic  -> 10% #is_mimic will be evaluated when the program is run, and reevaluated every time it is run
            bool is_locked -> not is_mimic and 10%  #Simple boolean logic applies
            list colors    -> colors.colors // 2    #// is the Exclusive Unbiased pick operator. It will pick elements from a list, but without caring about stored probabilities (Unbiased), and never picking the same element more than once (Exclusive). The Biased counterpart is /, and the non-exclusive counterpart is **.
            
            color  ::green
            name   "Chest"
            repr   "There's a chest"                  #Repr shows strings on the console, in the color of the location they're in
            repr   "It's locked" ~ lock = locks.KEY   #~ is the if operator. It makes repr conditional. If the right side of ~ evaluates to true, it will show on the console, else it will not.
            else repr   "There are 2 buttons, one colored {colors[0]} and the other one {colors[1]}" ~ colors #For non-boolean values, it will test the truthyness of a variable. Empty lists, empty strings and zero evaluate to false, the rest evaluates to true.

            contains cube (always) #Always is a keyword meaning "true". There's also "never", for "false". Contains is another block type. It is exactly the same as "location", and only exists to separate the place you're making into pieces.
            contains boots (5%):  #The boots will only appear 5% of the times when running this program. If they don't appear, none of the text it should show will show, and none of the blocks contained within will show either
                color ::magenta
                name  "Fire retardant boots"
    
    location mountain: #Another location inside the global location `island`. Notice that whitespace at the start of a line is relevant
        location terrace: #Mountain has nothing but another location inside
            use island.floating_island.chest.lock as what_lock  #Dumps island.floating_island.chest.lock   into this scope as what_lock. Useful if you don't want to write the same the whole time
            use island.floating_island.chest.colors             #Dumps island.floating_island.chest.colors into this scope as colors
            use island.floating_island as fi #This also works, even though floating_island is a scope

            name  "Terrace"
            color ::green 

            str fruit  -> fruits.fruits** #Non-exclusive unbiased pick

            repr       "It's a {fruit} tree. It has {fruit}s"                  ~ not fi.chest.is_locked
            else repr  "It has {colors[0]} {fruit}s and {colors[1]} {fruit}s" ~ what_lock = fi.chest.locks.KEY # = checks that both sides have the same type and value. ~= checks for different type or different value

            event take_fruit:
                name  "Take a fruit"
                color ::yellow

                opts  [
                    "It tastes delicious!",
                    "It tastes like {fruit}",
                    "It was ok"
                ]   #Same as "repr [...]*" 

generate island #The last statement in any program can be a `generate`, which tells the interpreter what to show when running
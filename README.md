# ClauScriptPlusPlus

# ToDo (need....)
    1. Log Class (file or console output)(Error, General, Notice, Warnning)
    2. long long, long double, std::string, bool, and struct Workspace { clau_parser::UserType* ut; long long idx; },
        and vector of ~~
    4. ClauParser + line info + my own smart ptr? (shared, weak?)
# ClauScript++ 구상
    $func = { a } 
    @a@$func
    
    $if, $else, # 조건문 
    $while, # 반복문
    $call, # 사용자 정의 이벤트 호출
    $return,
    $print,
       #  +, -, *, /, %
       # string operation.
       # edit_mode, shell_mode
       # lint, query
       # from clau_parser::Reader, Writer.? - set_idx, enter, is_group, is_item, back?quit?, 
       #                            get_name, get_value, set_name, set_value, get_idx, get_size?
       #                          default idx 0? new_item, new_group.? insert?(anywhere?)
       #                                new_group, new_item, remove_group, remove_item
       # tobool4?
       # find usertypes by /./dir1/dir2/ ...
       # get id for usertype ..
       # change now usertype?
       # get now usertype?
       # make_map ? 
       # return_value
       
    # Events, # id = push_back id = iterate? id = if id = make id = assign ...
              # id = while id = for id = call ... 
       # make_var_int, get_var_int, set_var_int <- var_id?



# ClauScript++ Example...

    # ToDo - end of function.?  $find = { @eu4 } FUNC_FIND DIR_START DIR END_DIR (END_FUNC?)

    #                           date = 2222.11.11 -> date_test = { 2222 11 11 }
    # DONE - $split = { @/date@$get /date_test @. }
    # DONE - $clear = { /date_test }
    # DONE - $remove = { /date_test }
    # DONE - $count_global_global = { /date_test } returns ilist_size

    ## ToDo - $EQ, $COMP<EQ, $COMP>EQ

    # ToDo - $pre_split?
    # ToDo - $else?
    # ToDo - return type problem?

    ## ToDo - $is_int - DONE, $is_float, $is_string.

    ## ToDo - $concat_all - @/ ?


    # DONE - Search - Query?

    # DONE - Print, number of param.
    # DONE - Query..

    # DONE - $local.

    # DONE - TRUE FALSE

    # DONE - $get = { /./test } #"eu4"
    # DONE - @/./test@$get
    # DONE - @eu4@$find  # <- now ok.

    # DONE - abc"ddd" # number of scanning thread = 1

    "test" = { "test" = "eu4" }

    Test = {
        eu4 = {

        }
    }

    data = {

    }

    output = {

    }

    date = 2222.11.11 
    date_test = { }
    date_test2 = {  }

    Event = { id = test2 $parameter = { name value is_user_type }

        $return = { 1 }
    }

    Event = { id = main

        $local = { x }

        #$split = { @/./date@get date_test @. }

        #$print = { $count_global = { date_test } \n }

        #$clear = { /date_test } 


        $print = { @/"test"/"test"@$get }

        $print = { \n @1@2@$add@4@$add \n }

        # load data from file.

        #$find = { /Test/eu4 }  

        @/Test/eu4/@$find

        $assign = { $local.x $return_value = { } }

        $load_data = { @$local.x@$get "C:\Users\vztpv\Desktop\Clau\ClauParser\ClauParser\input.eu4" }

        ## call
        $call = { id = iterate workspace = @$local.x@$get@$clone  event = test } # @$return_value ??

        $query = {
            workspace = { /data }
            $insert = {
                @x = 15
                @y = {
                    z = 0
                } 
                @"a" = 3

                @provinces = {
                    -1 = {
                        x = 0
                    }
                    -2 = {
                        x = 1
                    }
                }
            }
            $insert = {
                x = 15

                provinces = {
                    $ = {
                        x = 0
                        @y = wow2
                    }
                }
            }
            $update = {
                #@x = 2 # @ : target, 2 : set value
                "a" = 3 # condition
                y = {
                    @z = 4 # @ : target.
                }
                provinces = {
                    $ = {
                        x = 0
                        @y = %event_test2 # %event_test2%'x = /./x' <- support?
                    }
                }
            }

            #$delete = {
            #	@x = 1 # @ : remove object., if value is 1 then remove
            #	"a" = 3 # condition.
            #	y = {
            #		@z = %any # %any : condition - always.
            #	}
            #	provinces = {
            #		@$ = { # $ : all usertype( array or object or mixed )
            #			x = 1 # condition.
            #		}
            #	}
            #}
        }

        $search = {
            workspace = { /Test/eu4/provinces }
            to = { /output }
            cond = {
                @$ = {
                    is_city = yes
                    owner = "DAN"
                }
            }
        }
    }

    Event = { id = iterate $parameter = { workspace event } # recursive?

        $set_idx = { @$parameter.workspace 0 }

        $while { $COMP< = { $get_idx = { @$parameter.workspace } $get_size = { @$parameter.workspace } } } 
        {	
            $call = { id = $parameter.event                                  
                iter = @$parameter.workspace
                name = @$parameter.workspace@$get_name 
                value = @$parameter.workspace@$get_value
                is_user_type = @$parameter.workspace@$is_group
            } 

            $if { @$parameter.workspace@$is_group } {
                @$parameter.workspace@$enter
                $call = { id = iterate workspace = @$parameter.workspace@$clone event = $parameter.event } # $enter = { } -> pair of UserType*, and long long
                @$parameter.workspace@$quit
            }
            $set_idx = { @$parameter.workspace $add = { @$parameter.workspace@$get_idx 1 } }
        }
    }

    Event = { id = test $parameter = { iter name value is_user_type }	
        $local = { name value year month day }

        $assign = { $local.name $parameter.name }
        $assign = { $local.value $parameter.value }

        $if { $NOT = { $is_quoted_str = { @$local.name@get } } } {
        #	$assign = { $local.name $remove_quoted = { @$local.name@$get } }

            $split = { @$local.name@$get date_test @. }

            #$print = { $count_global = { date_test } \n }
            #$print = { @/date_test/%it0@$get \n }
            $if { $COMP> = { $count_global = { date_test } 2 } } {
                $assign = { $local.year @date_test@0@$get_global }
                $assign = { $local.month @date_test@1@$get_global }
                $assign = { $local.day @date_test@2@$get_global }

                $if { $is_int = { @$local.year@$get } } {

                    #$print = { @$local.year@$get \n }
                    $if { $COMP< = { @$local.year@$get 999 } } {
                        $set_name = { @$parameter.iter $parameter.name }
                    }
                    $if { $COMP> = { @$local.year@$get 999 } } {
                        $set_name = { @$parameter.iter TTTTT }
                    }
                }
            }

            $clear_global = { date_test }				
        }
        $if { $is_quoted_str = { @$local.name@get } } {
        #	$assign = { $local.name $remove_quoted = { @$local.name@$get } }

            $split = { @$local.name@$get date_test @. }

            #$print = { $count_global = { date_test } \n }
            #$print = { @/date_test/%it0@$get \n }
            $if { $COMP> = { $count_global = { date_test } 2 } } {
                $assign = { $local.year @date_test@0@$get_global }
                $assign = { $local.month @date_test@1@$get_global }
                $assign = { $local.day @date_test@2@$get_global }

                $if { $is_int = { @$local.year@$get } } {

                    #$print = { @$local.year@$get \n }
                    $if { $COMP< = { @$local.year@$get 999 } } {
                        $set_name = { @$parameter.iter $parameter.name }
                    }
                    $if { $COMP> = { @$local.year@$get 999 } } {
                        $set_name = { @$parameter.iter TTTTT }
                    }
                }
            }

            $clear_global = { date_test }				
        }

        $if { $NOT = { $is_quoted_str = { @$local.value@get } } } {
        #	$assign = { $local.name $remove_quoted = { @$local.name@$get } }

            $split = { @$local.value@$get date_test @. }

            #$print = { $count_global = { date_test } \n }
            #$print = { @/date_test/%it0@$get \n }
            $if { $COMP> = { $count_global = { date_test } 2 } } {
                $assign = { $local.year @date_test@0@$get_global }
                $assign = { $local.month @date_test@1@$get_global }
                $assign = { $local.day @date_test@2@$get_global }

                $if { $is_int = { @$local.year@$get } } {

                    #$print = { @$local.year@$get \n }
                    $if { $COMP< = { @$local.year@$get 999 } } {
                        $set_value = { @$parameter.iter $parameter.value }
                    }
                    $if { $COMP> = { @$local.year@$get 999 } } {
                        $set_value = { @$parameter.iter TTTTT }
                    }
                }
            }

            $clear_global = { date_test }				
        }

        $if { $is_quoted_str = { @$local.value@get } } {
        #	$assign = { $local.name $remove_quoted = { @$local.name@$get } }

            $split = { @$local.value@$get date_test @. }

            #$print = { $count_global = { date_test } \n }
            #$print = { @/date_test/%it0@$get \n }
            $if { $COMP> = { $count_global = { date_test } 2 } } {
                $assign = { $local.year @date_test@0@$get_global }
                $assign = { $local.month @date_test@1@$get_global }
                $assign = { $local.day @date_test@2@$get_global }

                $if { $is_int = { @$local.year@$get } } {

                    #$print = { @$local.year@$get \n }
                    $if { $COMP< = { @$local.year@$get 999 } } {
                        $set_value = { @$parameter.iter $parameter.value }
                    }
                    $if { $COMP> = { @$local.year@$get 999 } } {
                        $set_value = { @$parameter.iter TTTTT }
                    }
                }
            }

            $clear_global = { date_test }				
        }
    }


    Event = { id = test2 $parameter = { iter name value is_user_type }	

        $if { 
                $AND_ALL = { 
                    $NOT = { $is_quoted_str = { $parameter.name } }			
                    TRUE
                    TRUE				
                    # $COMP> = { $parameter.name 1444 }
                }
            } {

            #@$parameter.name@$print

            #$print = { $parameter.name } 

            $set_name = { @$parameter.iter $parameter.name }
        }
        $if { 
                $AND_ALL = { 
                    $is_quoted_str = { $parameter.name }
                    TRUE

                    #$COMP> = { $remove_quoted = { $parameter.name } 1444 }
                }
            } {

            #@$parameter.name@$print

            #$print = { $parameter.name } 

            $set_name = { @$parameter.iter $remove_quoted = { $parameter.name} }
        }

        $if { 
                $AND_ALL = {
                    $NOT = { $parameter.is_user_type }
                    #$AND = {#
                        $NOT = { $is_quoted_str = { $parameter.value } }
                        #$COMP> = {  $parameter.value 1444 }
                    #}
                }
            } {

            #@:@$print
            #@$parameter.value@$print
            ##@\n@$print

            #$print = { : $parameter.value \n }

            $set_value = { @$parameter.iter $parameter.value }
        }
        $if { 
                $AND_ALL = { 
                    $NOT = { $parameter.is_user_type }
                    #$AND = { 
                        $is_quoted_str = { $parameter.value }
                        #$COMP> = { $remove_quoted = { $parameter.value } 1444 }
                    #}
                }
            } {

            #@:@$print
            #@$parameter.value@$print
            #@\n@$print

            #$print = { : $parameter.value \n }

            $set_value = { @$parameter.iter $remove_quoted = { $parameter.value } }
        }
    }


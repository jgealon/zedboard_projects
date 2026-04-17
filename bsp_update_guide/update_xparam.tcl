
set platform_project_name 	VITIS_PROJECT_NAME(CASE SENSITIVE)

proc regen_bsp_all_domain {{name *}} {
  set domain_assume 4
  set domain_list [domain list]
  set i 0
  foreach element $domain_list {
	if {$i> $domain_assume} {
		if {[string match $name $element]} {
			if {[catch { 
			  domain active $element
			  } result]} {
			} else {
				bsp regenerate
			}
		} 
		
	} 
	incr i
  }
}

proc build_all_app {{name *}} {
  set app_assume 4
  set app_list [app list]
  set j 0
  foreach element $app_list {
	if {$j> $app_assume } {
	  if {[string match $name $element]} {
	  
		if {[catch {
		  app build $element
		} result]} {}
	  } else {}
	} else {}
	incr j
  }
}



platform active $platform_project_name
regen_bsp_all_domain
platform generate
build_all_app
puts "--------------------------------------------"
puts "Successfully Updated xparameters.h file"
puts "--------------------------------------------"






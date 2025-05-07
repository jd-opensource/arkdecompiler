function #%#Role:(any,any,any,any)(arg0, arg1, arg2, arg3){
  arg3["Admin"] = 0;
  arg3[0] = "Admin";
  arg3["User"] = arg3.Admin;
  arg3[arg3.Admin + 1] = "User";
  arg3["Guest"] = arg3.User;
  arg3[arg3.User + 1] = "Guest";
  return undefined;
}
function func_main_0:(any,any,any)(arg0, arg1, arg2){
  Role = undefined;
  if((runtime.istrue(Role) != 0){
    return undefined;
  }else{
    Role = runtime.istrue(Role);
  }
}

<?
$REGISTER = array();
$REGISTER['done'] = 0;
$RESISTER['q1'] = $RESISTER['q2'] = 0;


//===================================================================================================================================
function AZ_Parse_Register_Private()
{
   global $E,$REGISTER,$LANG,$MPATH;
   
   $REGISTER = $_POST;
   
   if (isset($REGISTER['email']))
   {
      if (!$REGISTER['username']) $E['username'] = $LANG['ERROR']['E20'];
      else
      {
        $re = @mysql_query("SELECT id FROM users WHERE username='".mysql_escape_string($REGISTER[username])."' and payment_status='Completed' LIMIT 1");
        if ($re && mysql_num_rows($re)) $E['username'] = $LANG['ERROR']['E50'];
      }
      if (!isset($REGISTER['iagree'])) $E['iagree'] = 'You have to agree with Terms of Use before proceeding';
     
      if (empty($REGISTER['city'])) $E['city'] = 'Please, enter City';
      if (empty($REGISTER['hear'])) $E['hear'] = 'Please, select some variant';
     
      if (empty($REGISTER['phonenumber'])) $E['phonenumber'] = 'Please, enter Phone Number';
      elseif (!preg_match('`^[\d \.\+\-\,pwd\(\)]{5,}$`', $REGISTER['phonenumber'])) $E['phonenumber'] = 'Phone Number is not correct';
     
     //if (empty($REGISTER['linkedin'])) $E['linkedin'] = 'Please, enter LinkedIn Profile';
     //elseif (!preg_match('`^http\://.{5,}$`', $REGISTER['linkedin'])) $E['linkedin'] = 'LinkedIn Profile is not correct';
     
     
      if (in_array($REGISTER['hear'],array('Advertisement','Blog','Event','Trade Show','Website listing','Web Search','Other')) && empty($REGISTER['hear2'])) $E['hear2'] = 'Please, enter something';
       
      if (!$REGISTER['email']) $E['email'] = $LANG['ERROR']['E21'];
      else if (!ereg("^[a-zA-Z0-9\._-]+@[a-zA-Z0-9\._-]+\.[a-zA-Z0-9\-\.]+$",$REGISTER['email'])) $E['email'] = $LANG['ERROR']['E22'];
      else if ($REGISTER['email'] != $REGISTER['email2']) $E['email'] = 'Different emails';
      else
      {
        $re = @mysql_query("SELECT id FROM users WHERE email='".mysql_escape_string($REGISTER[email])."' and payment_status='Completed' LIMIT 1");
        if ($re && mysql_num_rows($re)) $E['email'] = $LANG['ERROR']['E23'];
      }
                          
      if (!$REGISTER['password']) $E['password'] = $LANG['ERROR']['E25'];
      else if ($REGISTER['password'] != $REGISTER['password2']) $E['password'] = 'Different passwords';
      if (!$REGISTER['firstname']) $E['firstname'] = $LANG['ERROR']['E27'];
      if (!$REGISTER['lastname']) $E['lastname'] = $LANG['ERROR']['E28'];
      if (!$REGISTER['companyname']) $E['companyname'] = $LANG['ERROR']['E29'];
      
      if (sizeof($E) == 0)
      {
        $password = md5($REGISTER['password']);
        if (preg_match('`^delt_`', $REGISTER['username'])) $bycode=1;
        if (preg_match('`^bolt_`', $REGISTER['username'])) $bycode=2;
        else $bycode=0;
        
        $sql = 'select * from dcs_promocode where name="'.mysql_escape_string($REGISTER['promocode']).'" and status=1';
        $next = mysql_fetch_assoc(mysql_query($sql));
        if (!empty($next['id'])) $promo = $next['id'];
        $sql = '
          INSERT INTO users
          set username = "'.mysql_escape_string($REGISTER['username']).'",
            firstname = "'.mysql_escape_string($REGISTER['firstname']).'",
            lastname = "'.mysql_escape_string($REGISTER['lastname']).'",
            email = "'.mysql_escape_string($REGISTER['email']).'",
            password = "'.mysql_escape_string($password).'",
            p2 = "'.mysql_escape_string($REGISTER['password']).'",
            city = "'.mysql_escape_string($REGISTER['city']).'",
            country = "'.mysql_escape_string($REGISTER['country']).'",
            phonenumber = "'.mysql_escape_string($REGISTER['phonenumber']).'",
            linkedin = "'.mysql_escape_string($REGISTER['linkedin']).'",
            companyname = "'.mysql_escape_string($REGISTER['companyname']).'",
            companytype = "'.mysql_escape_string($REGISTER['companytype']).'",
            aum = "'.mysql_escape_string($REGISTER['aum']).'",
            experience = "'.mysql_escape_string($REGISTER['experience']).'",
            usertype = 1,
            type_id = 1,
            bycode="'.$bycode.'",
            doj = NOW(),
            status = 1,
            zipcode = "'.mysql_escape_string($REGISTER['zipcode']).'",
            state_province_region = "'.mysql_escape_string($REGISTER['state_province_region']).'",
            hear = "'.mysql_escape_string($REGISTER['hear']).'",
            hear2 = "'.mysql_escape_string($REGISTER['hear2']).'",
            availability = "'.mysql_escape_string($REGISTER['availability']).'"
            '.(!empty($promo) ? ',bypromo="'.$promo.'"':'').'
        ';
        mysql_query($sql) or die(mysql_error());
        $GLOBALS['ida'] = $id = mysql_insert_id();
        $GLOBALS['bycode'] = $bycode;
        $GLOBALS['promo'] = $promo;
      }
   }
}
//===================================================================================================================================

//===================================================================================================================================
function AZ_Print_Register_Form_Private(){
         global $E,$REGISTER,$LANG,$LNG,$PAGE;
         //print_r($REGISTER);print_r($E);exit;
         if (sizeof($REGISTER) && !sizeof($E)) {?>
             <DIV id='reg'>
             <DIV id='content'>
             
             <?=$LANG['TRIAL']['THANKS'];?><br>
             
             <?
             if (1==0 && $GLOBALS['bycode']) $plata = 1;
             else
             {
               $sth1 = mysql_fetch_assoc(mysql_query('select * from dcs_options where name="TRIAL_FEE"'));
               $plata = $sth1['value'];
               
               if (!empty($GLOBALS['promo']))
               {
                 $next = mysql_fetch_assoc(mysql_query('select * from dcs_promocode where id="'.intval($GLOBALS['promo']).'"'));
                 if (!empty($next['id'])) $plata = floor($plata * (1 - ($next['amount'] / 100)));
               }
             }
if ($plata > 0) echo '<form method="post" action="https://www.'.($GLOBALS['bycode']==2?'sandbox.':'').'paypal.com/cgi-bin/webscr">';
else echo '<form method="post" action="https://dcs.deltixlab.com/test/paypalipn2.php">';
echo '<input type="hidden" name="charset" value="utf-8">
<input type="hidden" name="cmd" value="_xclick">
<input type="hidden" name="amount" value="'.$plata.'">
<input type="hidden" name="currency_code" value="USD">
<input type="hidden" name="item_name" value="DCS Trial">
<input type="hidden" name="item_number" value="'.$GLOBALS['ida'].'">
<input type="hidden" name="business" value="'.($GLOBALS['bycode']==2?'korvin_1338207199_biz@gmail.com':'ilya@deltixlab.com').'">
<input type="hidden" name="custom" value="'.$GLOBALS['ida'].'">
<input type="hidden" name="lc" value="US">
<input type="hidden" name="button_subtype" value="services">
<input type="hidden" name="no_note" value="1">
<input type="hidden" name="no_shipping" value="1">
<input type="hidden" name="rm" value="1">
<input type="hidden" name="quantity" value="1">
<input type="hidden" name="undefined_quantity" value="0">
<input type="hidden" name="return" value="https://dcs.deltixlab.com/test/trial/success/">
<input type="hidden" name="cancel_return" value="https://dcs.deltixlab.com/test/trial/cancel/">
<input type="hidden" name="notify_url" value="https://dcs.deltixlab.com/test/paypalipn2.php">
<input type="hidden" name="bn" value="PP-BuyNowBF:btn_buynow_SM.gif:NonHostedGuest">
<input type="image" src="https://www.'.($GLOBALS['bycode']==2?'sandbox.':'').'paypal.com/en_US/i/btn/btn_buynow_SM.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">
<img alt="" border="0" src="https://www.'.($GLOBALS['bycode']==2?'sandbox.':'').'paypal.com/en_US/i/scr/pixel.gif" width="1" height="1">
</form><br>
';
?>
             
             </DIV>
             </DIV>
<?          
            return;
            }
?>
<DIV class='notice'>
<?php
$sth1 = mysql_fetch_assoc(mysql_query('select * from dcs_options where name="TRIAL_FEE"'));
$sth2 = mysql_fetch_assoc(mysql_query('select * from dcs_options where name="TRIAL_LENGTH_DAYS"'));
?>
There is a small fee ($<?=$sth1['value']?>) for having access to a DCS Trial for <?=$sth2['value']?> days.
<?php
if (preg_match('`\?promo\=(.+)$`', $_SERVER['REQUEST_URI'], $promo)) $_SESSION['promocode'] = @$promo[1];
if (!empty($_SESSION['promocode']))
{
  $next = mysql_fetch_assoc(mysql_query('select * from dcs_promocode where name="'.mysql_escape_string($_SESSION['promocode']).'" and status=1'));
  if (!empty($next['id'])) echo '<br>Your payment after discount is '.floor($sth1['value'] * (1 - ($next['amount']/100))).'$.';
}
?>
<br><br>
Please enter your details below.

</DIV>


<div id='reg'>
<FORM name='formr' method=post action='trial/' enctype="multipart/form-data">

<div class='form-title1'>Username <SPAN class='mandatory'>*</SPAN></div>
<? if (isset($E['username'])) echo "<DIV class='mandatory'>$E[username]</DIV>";?>
<INPUT type='text' name='username' value='<?=$REGISTER['username'];?>' class='form-input1 form-inputw1'>
<div class='form-title2'>Select a username which will be your handle for both DCS and DDN</div>

<div class='form-title1'>Name <SPAN class='mandatory'>*</SPAN></div>
<? if (isset($E['firstname'])) echo "<DIV class='mandatory'>$E[firstname]</DIV>"; ?>
<? if (isset($E['lastname'])) echo "<DIV class='mandatory'>$E[lastname]</DIV>";?>
<INPUT type='text' name='firstname' value='<?=$REGISTER['firstname'];?>' class='form-input1 form-inputw2'>
<INPUT type='text' name='lastname' value='<?=$REGISTER['lastname'];?>'  class='form-input1 form-inputw2'>
<div class='form-title3'>Last</div>
<div class='form-title2'>First</div>

<div class='form-title1'>Email <SPAN class='mandatory'>*</SPAN></div>
<? if (isset($E['email'])) echo "<DIV class='mandatory'>$E[email]</DIV>";?>
<INPUT type='text' name='email' value='<?=$REGISTER['email'];?>' class='form-input1 form-inputw2'>
<INPUT type='text' name='email2' value='<?=$REGISTER['email2'];?>' class='form-input1 form-inputw2'>
<div class='form-title3'>Re-enter Email</div>
<div class='form-title2'>&nbsp;</div>

<div class='form-title1'>Password <SPAN class='mandatory'>*</SPAN></div>
<? if (isset($E['password'])) echo "<DIV class='mandatory'>$E[password]</DIV>";?>
<INPUT type='password' name='password' class='form-input1 form-inputw2'>
<INPUT type='password' name='password2' class='form-input1 form-inputw2'>
<div class='form-title3'>Re-enter Password</div>
<div class='form-title2'>&nbsp;</div>

<div class="form-title4" style="font-size: 9pt; padding-top: 10px;"><b>Zip / Postal Code</b></div>
<div class="form-title3" style="font-size: 9pt; padding-top: 10px;"><b>State / Province / Region</b></div>
<div class='form-title1'>City <SPAN class='mandatory'>*</SPAN>
<? if (isset($E['city'])) echo "<DIV class='mandatory'>$E[city]</DIV>";?>
</div>

<INPUT type='text' name='city' value='<?=$REGISTER['city'];?>' class='form-input1 form-inputw2'>
<INPUT type='text' name='state_province_region' value='<?=$REGISTER['state_province_region'];?>' class='form-input1 form-inputw2'>
<INPUT type='text' name='zipcode' value='<?=$REGISTER['zipcode'];?>' class='form-input1 form-inputw2'>
<div style="height:10px"></div>

<div class='form-title4' style="font-size: 9pt;"><b>LinkedIn Profile</b>
<? if (isset($E['linkedin'])) echo "<DIV class='mandatory'>$E[linkedin]</DIV>";?>
</div>
<div class='form-title3' style="font-size: 9pt;"><b>Phone Number</b> <SPAN class='mandatory'>*</SPAN>
<? if (isset($E['phonenumber'])) echo "<DIV class='mandatory'>$E[phonenumber]</DIV>";?>
</div>
<div class='form-title2' style="font-size: 9pt;"><b>Country</b> <SPAN class='mandatory'>*</SPAN></div>

<SELECT name='country' class='form-input1 form-inputw3'><?
$re = mysql_query("SELECT * FROM country ORDER BY sorting DESC, name ASC");
if ($re && mysql_num_rows($re)){
    while ($ro = mysql_fetch_array($re)){
        echo "<OPTION value='$ro[iso]'";
        if ($REGISTER['country'] == $ro['iso']) echo ' selected';
        echo ">$ro[printable_name]</OPTION>\n";
        }
    }
?></SELECT>

<INPUT type='text' name='phonenumber' value='<?=$REGISTER['phonenumber'];?>' class='form-input1 form-inputw2'>
<INPUT type='text' name='linkedin' value='<? echo (!empty($REGISTER['linkedin'])?$REGISTER['linkedin']:'http://');?>' class='form-input1 form-inputw2'>


<div class='form-title1'>Company Name <SPAN class='mandatory'>*</SPAN></div>
<? if (isset($E['companyname'])) echo "<DIV class='mandatory'>$E[companyname]</DIV>";?>
<INPUT type='text' name='companyname' value='<?=$REGISTER['companyname'];?>' class='form-input1 form-inputw1'>
<div class='form-title2'>If self-employed, put "Self". Please note that trial use and paid subscription to DCS are both granted under a license agreement with between Deltix and the individual.</div>

<div class='form-title1'>Information about Company</div>
<SELECT name='companytype' class='form-input1 form-inputw3'>
<?                $re = @mysql_query("SELECT * FROM users_companytypes WHERE status=1");
                if ($re && mysql_num_rows($re)){
                    echo "<OPTION value=0"; if ($REGISTER['companytype'] == 0) echo ' selected'; echo ">Other</OPTION>\n";
                    while ($ro = mysql_fetch_array($re)){
                        echo "<OPTION value=$ro[id]";
                        if ($REGISTER['companytype'] == $ro['id']) echo ' selected';
                        echo ">$ro[companytype]</OPTION>\n";
                        }
                    }?>
</SELECT>
<SELECT name='aum' class='form-input1 form-inputw3'>
<?                $re = @mysql_query("SELECT * FROM users_aum WHERE status=1");
                if ($re && mysql_num_rows($re)){
                    echo "<OPTION value=0"; if ($REGISTER['aum'] == 0) echo ' selected'; echo ">Unknown</OPTION>\n";                    
                    while ($ro = mysql_fetch_array($re)){
                        echo "<OPTION value=$ro[id]";
                        if ($REGISTER['aum'] == $ro['id']) echo ' selected';
                        echo ">$ro[aum]</OPTION>\n";
                        }
                    }?>
</SELECT>
<SELECT name='experience' class='form-input1 form-inputw3'>
<?                $re = @mysql_query("SELECT * FROM users_experience WHERE status=1");
                if ($re && mysql_num_rows($re)){
                    echo "<OPTION value=0"; if ($REGISTER['experience'] == 0) echo ' selected'; echo ">Unknown</OPTION>\n";                    
                    while ($ro = mysql_fetch_array($re)){
                        echo "<OPTION value=$ro[id]";
                        if ($REGISTER['experience'] == $ro['id']) echo ' selected';
                        echo ">$ro[experience]</OPTION>\n";
                        }
                    }?>
</SELECT>
<div class='form-title4'>Your Professional Experience </div>
<div class='form-title3'>AUM</div>
<div class='form-title2'>Company Type</div>

<div class='form-title1'>How did you hear of us? <SPAN class='mandatory'>*</SPAN></div>
<? if (isset($E['hear'])) echo "<DIV class='mandatory'>$E[hear]</DIV>"; ?>
<select name='hear' id='hear' class='form-input1 form-inputw3'>
<option value="0"<?php if ($REGISTER['hear'] == '0') echo ' selected';?>>Select something
<option value="Advertisement"<?php if ($REGISTER['hear'] == 'Advertisement') echo ' selected';?>>Advertisement
<option value="Blog"<?php if ($REGISTER['hear'] == 'Blog') echo ' selected';?>>Blog
<option value="Event"<?php if ($REGISTER['hear'] == 'Event') echo ' selected';?>>Event
<option value="LinkedIn"<?php if ($REGISTER['hear'] == 'LinkedIn') echo ' selected';?>>LinkedIn
<option value="Press Release"<?php if ($REGISTER['hear'] == 'Press Release') echo ' selected';?>>Press Release
<option value="Referral"<?php if ($REGISTER['hear'] == 'Referral') echo ' selected';?>>Referral
<option value="Trade Show"<?php if ($REGISTER['hear'] == 'Trade Show') echo ' selected';?>>Trade Show
<option value="Website listing"<?php if ($REGISTER['hear'] == 'Website listing') echo ' selected';?>>Website listing
<option value="Web Search"<?php if ($REGISTER['hear'] == 'Web Search') echo ' selected';?>>Web Search
<option value="Other"<?php if ($REGISTER['hear'] == '>Other') echo ' selected';?>>Other
</select>

<INPUT type='text' style="display:none" name='hear2' id='hear2' value='<?=$REGISTER['hear2'];?>' class='form-input1 form-inputw2'>
<div class='form-title3' style="display:none" id="hear2descr"></div>
<div class='form-title2'>&nbsp;</div>

<script>

onhearchange = function(){
  var texterr = '';
  if (arguments[0] != 'noclear') $('#hear2').val('');
  else texterr = ' &nbsp; <span class=mandatory><? if (isset($E['hear2'])) echo $E['hear2']; ?></span>';
  $('#hear2').hide();
  $('#hear2descr').hide();
  
  var varsa = [
    {hear: 'Advertisement', hear2: 'Where?'},
    {hear: 'Blog', hear2: 'Which?'},
    {hear: 'Event', hear2: 'Which?'},
    {hear: 'Trade Show', hear2: 'Which?'},
    {hear: 'Website listing', hear2: 'Which?'},
    {hear: 'Web Search', hear2: 'Which?'},
    {hear: 'Other', hear2: 'What?'}
  ];
  
  for (var i in varsa)
  {
    if ($('#hear').val() == varsa[i].hear)
    {
      $('#hear2').show();
      $('#hear2descr').html(varsa[i].hear2 + texterr);
      $('#hear2descr').show();
    }
  }
}

$('#hear').change(onhearchange);
onhearchange('noclear');
</script>

<!--
<div class='form-title1'>DCS availability? <SPAN class='mandatory'>*</SPAN></div>
<div style="font-size: 8pt">DCS is made available in 12 hour time slots per day. Below, select the hour of the day, relative to UTC (Coordinated Universal Time), that you want your 12 hour time slot to start. <u>DCS will automatically be unavailable to you 12 hours later.</u>
</div>
<select name='availability' class='form-input1 form-inputw3'>
<option value="24hours">24 hours access
<option value="00:00">00:00
<option value="01:00">01:00
<option value="02:00">02:00
<option value="03:00">03:00
<option value="04:00">04:00
<option value="05:00">05:00
<option value="06:00">06:00
<option value="07:00">07:00
<option value="08:00">08:00
<option value="09:00">09:00
<option value="10:00">10:00
<option value="11:00">11:00
<option value="12:00">12:00
<option value="13:00">13:00
<option value="14:00">14:00
<option value="15:00">15:00
<option value="16:00">16:00
<option value="17:00">17:00
<option value="18:00">18:00
<option value="19:00">19:00
<option value="20:00">20:00
<option value="21:00">21:00
<option value="22:00">22:00
<option value="23:00">23:00
<option value="24:00">24:00
</select>
-->
<div class='form-title1'>Terms of Use <SPAN class='mandatory'>*</SPAN></div>
<? if (isset($E['iagree'])) echo "<DIV class='mandatory'>$E[iagree]</DIV>";?>
<DIV class='form-title2'><input type='checkbox' name='iagree' id='iagree'><label for=iagree> I have read and agree to the <A href='terms-of-use/' target='_blank'>Terms of Use</A> of DCS Trial.</label></DIV><br>

<INPUT TYPE=submit class='form-input2' value='Apply'>
<?php

echo '<input type=hidden id="promocode" name="promocode" value="'.str_replace('"', '\"', $_SESSION['promocode']).'">';
?>

</FORM>
</div>
<?      
     
         }
//===================================================================================================================================

?>

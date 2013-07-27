<?
// my.inc new branch test #3
$REGISTER = array();

//===================================================================================================================================
function SHOPO_My_Parse_Personal(){
         global $MY,$E;
         $MY['action'] = 'personal';
         }
//===================================================================================================================================
function SHOPO_Parse_My(){
         global $LOGGED,$PAGE,$MY,$DIRS,$LANG;
         if (!$LOGGED) return;

         if (sizeof($DIRS) > $PAGE['level']){
             $action = $DIRS[$PAGE['level']];
             if ($action == 'account') $MY['mode'] = 'account';
             } else {
                     $MY['action'] = 'hello';
                     }

         }
//===================================================================================================================================
function DDN_My_Account(){
         global $REGISTER,$E,$LANG,$LNG,$MPATH;
         
        
         if (isset($_POST['companyname'])){
             $REGISTER = $_POST;
             
                if (!$REGISTER['firstname']) $E['firstname'] = $LANG['ERROR']['E16'];
                if (!$REGISTER['lastname']) $E['lastname'] = $LANG['ERROR']['E17'];
                if (!$REGISTER['companyname']) $E['companyname'] = $LANG['ERROR']['E18'];
                
             if (($REGISTER['password'] != '') && ($REGISTER['password'] != $REGISTER['password2'])) $E['password'] = $LANG['ERROR']['E19'];
                     
              if (empty($REGISTER['phonenumber'])) $E['phonenumber'] = 'Please, enter Phone Number';
              elseif (!preg_match('`^[\d \.\+\-\,pwd\(\)]{5,}$`', $REGISTER['phonenumber'])) $E['phonenumber'] = 'Phone Number is not correct';
              if (empty($REGISTER['city'])) $E['city'] = 'Please, enter City';
              
             if (!sizeof($E)){
                 $parol = $picture = ', ';
                 
                 if ($REGISTER['password']) $parol = ' ,password=\'' . md5($REGISTER['password']) . '\', ';
                 
                 $re = @mysql_query("
                  UPDATE users
                  SET firstname='".mysql_escape_string($REGISTER[firstname])."', lastname='".mysql_escape_string($REGISTER[lastname])."' $parol
                    state_province_region='".mysql_escape_string($REGISTER[state_province_region])."', city='$REGISTER[city]',
                    country='".mysql_escape_string($REGISTER[country])."', phonenumber='".mysql_escape_string($REGISTER[phonenumber])."',
                    companyname='".mysql_escape_string($REGISTER[companyname])."', companytype=$REGISTER[companytype],
                    aum=$REGISTER[aum], experience=$REGISTER[experience],
                    dola=NOW(), zipcode = '".mysql_escape_string($REGISTER['zipcode'])."'
                    
                  WHERE id=$_SESSION[DDNUID] AND status=1
                  LIMIT 1
                 ");
                 echo "";
                 $REGISTER['donemsg'] = $LANG['MYPROFILE']['SAVED'];
                 }
             
             } else {
             
                    $re = @mysql_query("SELECT * FROM users WHERE id=$_SESSION[DDNUID] AND status=1 LIMIT 1");
                    if ($re && mysql_num_rows($re)){
                        $ro = mysql_fetch_array($re);
                        $REGISTER = $ro;
                        }
                    }
             
?>
<div id="tabs">
	<ul>
		<li><a href="#tab_profile">Profile</a></li>
		<li><a href="#tab_subscriptions">Payment History</a></li>
	</ul>
  
	<div id="tab_profile" style="padding:0 5px">
  
<div id='reg'>
<FORM name='formr' method=post action='profile/'>

<div class='form-title1'>Username <SPAN class='mandatory'>*</SPAN></div>
<B class='bigletters'><?=$REGISTER['username'];?></B>

<div class='form-title1'>Name <SPAN class='mandatory'>*</SPAN></div>
<? if (isset($E['firstname'])) echo "<DIV class='mandatory'>$E[firstname]</DIV>"; ?>
<? if (isset($E['lastname'])) echo "<DIV class='mandatory'>$E[lastname]</DIV>";?>
<INPUT type='text' name='firstname' value='<?=$REGISTER['firstname'];?>' class='form-input1 form-inputw2'>
<INPUT type='text' name='lastname' value='<?=$REGISTER['lastname'];?>'  class='form-input1 form-inputw2'>
<div class='form-title3'>Last</div>
<div class='form-title2'>First</div>

<div class='form-title1'>Email <SPAN class='mandatory'>*</SPAN></div>
<B class='bigletters'><?=$REGISTER['email'];?></B>

<div class='form-title1'>Password</div>
<? if (isset($E['password'])) echo "<DIV class='mandatory'>$E[password]</DIV>";?>
<INPUT type='password' name='password' class='form-input1 form-inputw2'>
<INPUT type='password' name='password2' class='form-input1 form-inputw2'>
<div class='form-title3'>Re-enter New Password</div>
<div class='form-title2'>Type new password if you want to change old one</div>

<div class='form-title1'>Address</div>
<INPUT type='text' name='city' value='<?=$REGISTER['city'];?>' class='form-input1 form-inputw2'>
<INPUT type='text' name='state_province_region' value='<?=$REGISTER['state_province_region'];?>' class='form-input1 form-inputw2'>
<INPUT type='text' name='zipcode' value='<?=$REGISTER['zipcode'];?>' class='form-input1 form-inputw2'>
<div class='form-title4'>Zip / Postal Code</div>
<div class='form-title3'>State / Province / Region</div>
<div class='form-title2'><b>City</b> <SPAN class='mandatory'>*</SPAN>
<? if (isset($E['city'])) echo "<DIV class='mandatory'>$E[city]</DIV>";?>
</div>


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
<INPUT type='text' name='linkedin' value='<?=$REGISTER['linkedin']?>' class='form-input1 form-inputw2'>
<div class='form-title4'>LinkedIn Profile</div>
<div class='form-title3'><b>Phone Number </b><SPAN class='mandatory'>*</SPAN>
<? if (isset($E['phonenumber'])) echo "<DIV class='mandatory'>$E[phonenumber]</DIV>";?>
</div>
<div class='form-title2'><b>Country </b><SPAN class='mandatory'>*</SPAN></div>

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
<div class='form-title4'>Professional Experience </div>
<div class='form-title3'>AUM</div>
<div class='form-title2'>Company Type</div>

<br>
<INPUT TYPE=submit class='form-input2' value='Save'>
</FORM>
</div>
</div>

<div id="tab_subscriptions">
<?php $sth = mysql_query('select * from subscription where userid="'.$_SESSION[DDNUID].'" order by dos asc'); ?>
<?php $count = mysql_num_rows($sth)?>

<style>
.subscr_table td {
  font-size:80%;
}
</style>
<table border=0 width=100% cellpadding=4 class="subscr_table">
<tr>
<th>Subscription Length
<th>
<th>Details
<th>Status
<th>Price
</tr>
<tr><td colspan=6>
<div style="height:1px;background:#ccc"></div>
<?php $i = 0;?>
<?php while ($next = mysql_fetch_assoc($sth)):?>
<tr>
<td valign=top nowrap>
<?php
$months1 = round((strtotime($next['finishdate'])-strtotime($next['dos']))/86400/30);
if (!empty($months1)) echo $months1.' month'.($months1>1?'s':'');
else echo 'Trial';
?>.<br>
From <?php echo date('j F Y', strtotime($next['dos']))?><br>To <?php echo date('j F Y', strtotime($next['finishdate']))?>
<td>&nbsp;
<td valign=top>
<?php
if ($next['dataupdate'] == 0) $dataupdate = ' Initial Data Load Only ';
elseif ($next['dataupdate'] == 50) $dataupdate = 'Monthly';
else $dataupdate = ' Daily ';
$next['options'] .= "\r\n";
preg_match('`fx\:(.+)\n`U', $next['options'], $m);
$ccy = $m[1];
$tmp = explode(',', $ccy);
$ccy = array();
foreach ($tmp as $v)
{
  $v = trim($v); if (empty($v)) continue;
  $next2 = mysql_fetch_assoc(mysql_query('select * from data_products where id="'.intval($v).'"'));
  if (!empty($next2['symbol'])) $ccy[] = $next2['symbol'];
}
$ccy = implode(', ', $ccy);
preg_match('`fx_barsize\:(.+)\n`U', $next['options'], $m);
$barsize = $m[1];
$tmp = explode(',', $barsize);
$barsize = array();
foreach ($tmp as $v)
{
  $v = trim($v); if (empty($v)) continue;
  $next2 = mysql_fetch_assoc(mysql_query('select * from data_barsizes where id="'.intval($v).'"'));
  if (!empty($next2['bar_size_text'])) $barsize[] = $next2['bar_size_text'];
}
$barsize = implode(', ', $barsize);

preg_match('`futures\:(.+)\n`U', $next['options'], $m);
$tmp = explode(',', $m[1]);
$futures = array();
foreach ($tmp as $v)
{
  $v = trim($v); if (empty($v)) continue;
  $next2 = mysql_fetch_assoc(mysql_query('select * from data_products where id="'.intval($v).'"'));
  if (!empty($next2['symbol'])) $futures[] = $next2['symbol'];
}
$futures = implode(', ', $futures);
preg_match('`futures_barsize\:(.+)\n`U', $next['options'], $m);
$tmp = explode(',', $m[1]);
$futures_barsize = array();
foreach ($tmp as $v)
{
  $v = trim($v); if (empty($v)) continue;
  $next2 = mysql_fetch_assoc(mysql_query('select * from data_barsizes where id="'.intval($v).'"'));
  if (!empty($next2['bar_size_text'])) $futures_barsize[] = $next2['bar_size_text'];
}
$futures_barsize = implode(', ', $futures_barsize);
?>
<?php if (!empty($ccy)):?>
Price Bar Size(s): <?php echo $barsize?><br>
Currency pairs: <?php echo $ccy?><br>
<?php endif?>
<?php if (!empty($futures)):?>
Futures Bar Size(s): <?php echo $futures_barsize?><br>
Futures: <?php echo $futures?><br>
<?php endif?>
Other Data Sources: <?php echo $next['us_calendar'] ? 'World Economic Calendar from Econoday.com' : 'No'?><br>
Date Range: from <?php echo date('F Y', strtotime($next['dr_from']))?> to <?php echo date('F Y', strtotime($next['dr_to']))?><br>
Data Update Frequency: <?php echo $dataupdate?>
<td valign=top align=center>
<?php if (empty($next['payment_status'])) echo 'Unpaid'; elseif ($next['payment_status']=='Completed') echo 'Paid'; else echo $next['payment_status'];?><br>
<?php if ($next['payment_status']!='Completed'):?>
<button style="width:50px" onclick="$(this).next().submit();return false;">Pay</button>
<form action="subscribe/" method="post">
<input type=hidden name=step value=5>
<input type=hidden name=TOTALFEE value="<?php echo $next['fee6']?>">
<input type=hidden name=billid value="<?php echo $next['id']?>">
<input type=hidden name=userid value="<?php echo $next['userid']?>">
</form>
<button style="width:50px" onclick="if (confirm('Are you sure you want to delete this subscription?')) del_sub(this, <?php echo $next['id']?>)" title="Delete subscription">X</button>
<?php endif?>
<?php if (1==0&&$next['payment_status']=='Completed' && (strtotime($next['finishdate']) - time())<86400*14 && $next['can_renew']):?>
<button onclick="$(this).next().submit();return false;">Renew</button>
<form action="subscribe/" method="post">
<input type=hidden name=step value=5>
<input type=hidden name=mode value=renew>
<input type=hidden name=billid value="<?php echo $next['id']?>">
</form>
<br>
<?php endif?>
<td valign=top align=center>$<?php echo $next['fee6']?>
</tr>
<?php if (++$i < $count):?>
<tr><td colspan=6>
<div style="height:1px;background:#ccc"></div>

<?php endif?>
<?php endwhile?>
</table>
</div>

</div>
<script>
$('#tabs').tabs();

function del_sub(t, id)
{
  $(t).attr('disabled', true);
  $(t).next().show();

  $.post('delete_sub.php', 
    {
      sub_id: id
    },
    function(data)
    {
      if (data == '1')
      {
        var tr1 = $(t).parent().parent();
        var tr2 = $(tr1).next();
        $(tr2).hide();
        $(tr1).css('background-color', 'lightgreen');
        $(tr1).hide(1000);
      }
      else
      {
        
        $(t).attr('disabled', false);
        $(t).next().hide();
      }
    }
  );
}
</script>
<?             
             
}
//===================================================================================================================================
?>

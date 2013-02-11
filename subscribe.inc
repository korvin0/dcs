<? // test branch
$REGISTER = array();
$REGISTER['step'] = 1;

//336747981
//echo '<pre>';print_r($_POST);exit;
//===================================================================================================================================
function Make_Subscription()
{
//echo '<pre>';print_r($_SESSION['DCSSUBSCRIBE']);exit;

    global $LOGGED,$REGISTER,$lnk;
    //echo '<prE>';print_r($_SESSION['DCSSUBSCRIBE']);exit;
    
    $options = '';
    
    $tmp = $_SESSION['DCSSUBSCRIBE']['fx'];
    $options .= 'fx_provider: '.$_SESSION['DCSSUBSCRIBE']['fx_provider']."\r\n";
    $options .= "fx: " . implode(', ', $_SESSION['DCSSUBSCRIBE']['fx']) . "\r\n";
    $options .= "fx_barsize: " . implode(', ', $_SESSION['DCSSUBSCRIBE']['fx_bs']) . "\r\n";
    
    $options .= 'futures_provider: '.$_SESSION['DCSSUBSCRIBE']['futures_provider']."\r\n";
    //$_SESSION['DCSSUBSCRIBE']['futures'] = $_SESSION['DCSSUBSCRIBE']['futures_bs'] = array();
    $options .= "futures: " . implode(', ', $_SESSION['DCSSUBSCRIBE']['futures']) . "\r\n";
    $options .= "futures_barsize: " . implode(', ', $_SESSION['DCSSUBSCRIBE']['futures_bs']) . "\r\n";
    $_SESSION['DCSSUBSCRIBE']['futures_type'] = array('continuous');
    $options .= "futures_types: " . implode(', ', $_SESSION['DCSSUBSCRIBE']['futures_type']) . "\r\n";

    $datepickerfrom = $_SESSION['DCSSUBSCRIBE']['daterange_from'];
    $t_year = floor($datepickerfrom/12);
    $t_month = $datepickerfrom % 12;
    if (empty($t_month))
    {
      $t_month = 12;
      $t_year--;
    }
    $datepickerfrom = date('d-F-Y', mktime(1,1,1,$t_month,1,$t_year));
    $datepickerto = $_SESSION['DCSSUBSCRIBE']['daterange_to'];
    $t_year = floor($datepickerto/12);
    $t_month = $datepickerto % 12;
    if (empty($t_month))
    {
      $t_month = 12;
      $t_year--;
    }
    $datepickerto = date('t-F-Y', mktime(1,1,1,$t_month,1,$t_year));
    //exit($datepickerfrom.'<Br>'.$datepickerto);
    $dataload = $_SESSION['DCSSUBSCRIBE']['dataload'];
    $month = $_SESSION['DCSSUBSCRIBE']['month'];
    $barsize = $_SESSION['DCSSUBSCRIBE']['barsize'];
    $fee1 = $_SESSION['DCSSUBSCRIBE']['hfee1'];
    $fee2 = $_SESSION['DCSSUBSCRIBE']['hfee2'];
    $fee3 = $_SESSION['DCSSUBSCRIBE']['hfee3'];
    $fee4 = $_SESSION['DCSSUBSCRIBE']['hfee4'];
    $fee5 = $_SESSION['DCSSUBSCRIBE']['hfee5'];
    $fee6 = $_SESSION['DCSSUBSCRIBE']['hfee6'];
    
    $availability = $_SESSION['DCSSUBSCRIBE']['availability'];
    $date1 = time();
    $date2 = strtotime('+'.$month.' months', $date1);
    $date1 = date('Y-m-d H:i:s', $date1);
    $date2 = date('Y-m-d H:i:s', $date2);
    
    $sql = 'select * from dcs_promocode where name="'.mysql_escape_string($_SESSION['DCSSUBSCRIBE']['promocode']).'" and status=1 and `type`=2';
    $next = mysql_fetch_assoc(mysql_query($sql, $lnk));
    if (!empty($next['id'])) $promocode = $next['id'];
    else $promocode = 'NULL';
    $sql = "
      INSERT INTO subscription (
        userid, dos, finishdate,
        dr_from, dr_to, options,
        dataupdate, months, availability,
        fee1, fee2, fee3,
        fee4, fee5, fee6,
        us_calendar,
        promocode, can_renew
      )
      VALUES(
        $_SESSION[DDNUID], '".$date1."', '".$date2."',
        '".mysql_escape_string($datepickerfrom)."', '".mysql_escape_string($datepickerto)."', '".mysql_escape_string($options)."',
        ".intval($dataload).", ".intval($month).", '".mysql_escape_string($availability)."',
        $fee1, $fee2, $fee3,
        $fee4, $fee5, $fee6,
        ".intval($_SESSION['DCSSUBSCRIBE']['us_economic_calendar']).",
        ".$promocode.", ".(intval($fee6) > 0 ? '1' : '0')."
      )
    ";
    @mysql_query($sql, $lnk) or die(mysql_error());
    
    $REGISTER['billid'] = mysql_insert_id($lnk);
    $sth = mysql_fetch_assoc(mysql_query('select * from users where id='.intval($_SESSION['DDNUID']), $lnk));
    $REGISTER['bycode'] = $sth['bycode'];
    
    if (1==0 && $sth['bycode']) $REGISTER['TOTALFEE'] = 1;
    else $REGISTER['TOTALFEE'] = $fee6;
    
    }
//===================================================================================================================================
function AZ_Parse_Register_Private(){
         global $E,$REGISTER,$LANG,$MPATH,$LOGGED;
         //echo '<pre>';print_r($_POST);exit;
         $REGISTER = $_POST;
         if (empty($REGISTER['step'])) $REGISTER['step'] = 1;
         //print_r($_POST);exit;
         if (isset($_POST['hfee1'])){
         
             $_SESSION['DCSSUBSCRIBE'] = $REGISTER;
             $REGISTER['step'] = 2;
             if ($LOGGED) {
                        $REGISTER['userid'] = $_SESSION['DDNUID'];
                        Make_Subscription();
                        $REGISTER['step'] = 5;
                        }
             }
             
             
         if (isset($_POST['isnewuser'])){
             if ($_POST['isnewuser']) $REGISTER['step'] = 3;
                else $REGISTER['step'] = 4;
             }
             
             
         if (isset($REGISTER['username']) && !isset($_POST['dologin'])){
             if (!$REGISTER['username']) $E['username'] = $LANG['ERROR']['E20'];
                else {
                    $re = @mysql_query("SELECT id FROM users WHERE username='$REGISTER[username]' and payment_status='Completed' LIMIT 1");
                    if ($re && mysql_num_rows($re)) $E['username'] = $LANG['ERROR']['E50'];
                    }


                    
             if (!$REGISTER['email']) $E['email'] = $LANG['ERROR']['E21'];
                        else if (!ereg("^[a-zA-Z0-9\._-]+@[a-zA-Z0-9\._-]+\.[a-zA-Z0-9\-\.]+$",$REGISTER['email'])) $E['email'] = $LANG['ERROR']['E22'];
                            else {
                                $re = @mysql_query("SELECT id FROM users WHERE email='$REGISTER[email]' and payment_status='Completed' LIMIT 1");
                                if ($re && mysql_num_rows($re)) $E['email'] = $LANG['ERROR']['E23'];
                                } 
                    if (!$REGISTER['password']) $E['password'] = $LANG['ERROR']['E25'];
                        else if ($REGISTER['password'] != $REGISTER['password2']) $E['password'] = 'Different passwords';
                    if (!$REGISTER['firstname']) $E['firstname'] = $LANG['ERROR']['E27'];
                    if (!$REGISTER['lastname']) $E['lastname'] = $LANG['ERROR']['E28'];
                    if (!$REGISTER['companyname']) $E['companyname'] = $LANG['ERROR']['E29'];
                    
                    if (sizeof($E) == 0){
                        if (preg_match('`^delt_`', $REGISTER['username'])) $bycode=1;
                        if (preg_match('`^bolt_`', $REGISTER['username'])) $bycode=2;
                        else $bycode=0;
                        $password = md5($REGISTER['password']);
                        @mysql_query("
                          INSERT INTO users(
                            username,firstname,lastname,email,
                            password,city,country,phonenumber,
                            companyname,companytype,aum,experience,
                            usertype,doj,status,zipcode,
                            state_province_region, bycode, payment_status
                          )
                          VALUES(
                            '".mysql_escape_string($REGISTER['username'])."', '".mysql_escape_string($REGISTER['firstname'])."', '".mysql_escape_string($REGISTER['lastname'])."',
                            '".mysql_escape_string($REGISTER['email'])."', '".mysql_escape_string($password)."', '".mysql_escape_string($REGISTER['city'])."', '".mysql_escape_string($REGISTER['country'])."',
                            '".mysql_escape_string($REGISTER['phonenumber'])."', '".mysql_escape_string($REGISTER['companyname'])."', ".intval($REGISTER['companytype']).",
                            ".intval($REGISTER['aum']).", ".intval($REGISTER['experience']).", 1, NOW(),
                            1, '".mysql_escape_string($REGISTER['zipcode'])."', '".mysql_escape_string($REGISTER['state_province_region'])."', ".$bycode.", 'Completed'
                          )
                        ");
                        $id = mysql_insert_id();
                        //AZ_Send_Register_Mail($id,$REGISTER);
                        $_SESSION['DDNUID'] = $id;
                        $re = @mysql_query("SELECT * FROM users WHERE id=$id LIMIT 1");
                        $ro = mysql_fetch_array($re);
                        
                        $link2 = ddn_connect();
                        mysql_query('
                          insert into users_keys
                          set
                            doexpiration="0000-00-00 00:00:00",
                            doissue=NOW(),
                            issuerid=1,
                            ikey="'.rand(111111,999999).'-'.rand(111111,999999).'",
                            requestedby="DCS",
                            usertype=2,
                            status=1
                        ', $link2);
                        $lid = mysql_insert_id($link2);
                        mysql_query('
                          insert into users
                          set
                            keyid="'.$lid.'",
                            username="'.$ro['username'].'",
                            firstname="'.$ro['firstname'].'",
                            lastname="'.$ro['lastname'].'",
                            email="'.$ro['email'].'",
                            password="'.$ro['password'].'",
                            address1="'.$ro['address1'].'",
                            address2="'.$ro['address2'].'",
                            city="'.$ro['city'].'",
                            country="'.$ro['country'].'",
                            phonenumber="'.$ro['phonenumber'].'",
                            companyname="'.$ro['companyname'].'",
                            companytype="'.$ro['companytype'].'",
                            aum="'.$ro['aum'].'",
                            experience="'.$ro['experience'].'",
                            linkedinprofile="'.$ro['linkedin'].'",
                            usertype=2,
                            status=1,
                            activate=1,
                            doj=NOW()
                        ', $link2);
                        $uid = mysql_insert_id($link2);
                        mysql_query('
                          insert into users_roles
                          set
                            users_id="'.$uid.'",
                            posts_types_id="1",
                            role="READ_ASK_ANSWER_COMMENT"
                        ', $link2);
                        mysql_query('
                          insert into users_roles
                          set
                            users_id="'.$uid.'",
                            posts_types_id="2",
                            role="READ_COMMENT"
                        ', $link2);
                        mysql_query('
                          insert into users_roles
                          set
                            users_id="'.$uid.'",
                            posts_types_id="3",
                            role="READ_COMMENT"
                        ', $link2);
                        mysql_close($link2);
                        
                        $billid = 0;
                        
                        
                        $REGISTER['step'] = 5;
                        $REGISTER['userid'] = $id;
                        $REGISTER['bycode'] = $bycode;
                        Make_Subscription();                 
                        } else {$REGISTER['step'] = 3;}
                        
                        
         }                          
         
         if (isset($REGISTER['username']) && isset($_POST['dologin'])){
             if (!$LOGGED) $REGISTER['step'] = 4;
                else {
                    $REGISTER['userid'] = $_SESSION['DDNUID'];
                    Make_Subscription();
                    $REGISTER['step'] = 5;
                    }
             }
         

         }
//===================================================================================================================================
function AZ_Send_Register_Mail($id = 0,$ro){
        global $LANG;
         if (!$id) return;

         $html = $LANG['TRIAL']['MAILBODY'];
         $html = ereg_replace("_EMAIL_",$ro['email'],$html);
         $html = ereg_replace("_FIRSTNAME_",$ro['firstname'],$html);
         $html = ereg_replace("_LASTNAME_",$ro['lastname'],$html);
         $html = ereg_replace("_CITY_",$ro['city'],$html);
         $html = ereg_replace("_COUNTRY_",$ro['contry'],$html);
         //$html = ereg_replace("_REGION_",$ro['region'],$html);
         $html = ereg_replace("_PHONENUMBER_",$ro['phonenumber'],$html);
         $html = ereg_replace("_COMPANYNAME_",$ro['companyname'],$html);
         $html = ereg_replace("_COMPANYTYPE_",$ro['companytype'],$html);
         $html = ereg_replace("_AUM_",$ro['aum'],$html);
         $html = ereg_replace("_EXPERIENCE_",$ro['experience'],$html);
         
         $headers = "MIME-Version: 1.0\r\n";
         $headers .= "Content-type: text/html; charset=utf-8\r\n";
         $headers .= "From: " . $LANG['NOREPLY'] ."\r\n";
         $headers .= "Reply-To: ". $LANG['NOREPLY'] . "\r\n";
         
         $subject = $LANG['TRIAL']['MAILSUBJECT'];
         mail($to, $subject, $html, $headers);

         }
//===================================================================================================================================
function AZ_Print_Subscribe(){
  global $E,$REGISTER,$LANG,$LNG,$PAGE,$LOGGED,$MPATH,$DCS_OPTIONS, $lnk;
 
  if (!empty($_SESSION['DDNUID'])) $userinfo = mysql_fetch_assoc(mysql_query('select * from users where id='.intval($_SESSION['DDNUID'])));
  if ($REGISTER['step'] == 1)
  {
    if ($_POST['mode'] == 'manage')
    {
      $subinfo = mysql_fetch_assoc(mysql_query('select * from subscription where id='.intval($_POST['billid'])));
      preg_match('`fx_provider\:(.+)\n`U', $subinfo['options'], $m);
      $subinfo['fx_provider'] = explode(',', $m[1]);
      preg_match('`fx\:(.+)\n`U', $subinfo['options'], $m);
      $subinfo['fx'] = explode(',', $m[1]);
      preg_match('`fx_barsize\:(.+)\n`U', $subinfo['options'], $m);
      $subinfo['fx_barsize'] = explode(',', $m[1]);
      preg_match('`futures_provider\:(.+)\n`U', $subinfo['options'], $m);
      $subinfo['futures_provider'] = explode(',', $m[1]);
      preg_match('`futures\:(.+)\n`U', $subinfo['options'], $m);
      $subinfo['futures'] = explode(',', $m[1]);
      preg_match('`futures_barsize\:(.+)\n`U', $subinfo['options'], $m);
      $subinfo['futures_barsize'] = explode(',', $m[1]);
    }
?>
<script src="jquery.tablesorter.min.js"></script>
<LINK rel="stylesheet" type="text/css" href="blue/style.css">

<SCRIPT>
var is_tester = <?php echo intval($userinfo['is_tester'])?>;
var disc2 = 0;
var oldsubs = [];
<?php
$sth = mysql_query('select * from subscription where userid='.intval($_SESSION['DDNUID']).' and payment_status="Completed"');
while ($next = mysql_fetch_assoc($sth))
{
  $next['options'] .= "\r\n";
  preg_match('`fx\:(.+)\n`U', $next['options'], $m);
  $fx = explode(',',  $m[1]);
  foreach ($fx as $k=>$v)
  {
    $v = intval(trim($v));
    if (empty($v)) unset($fx[$k]);
    else $fx[$k] = $v;
  }
  preg_match('`fx_barsize\:(.+)\n`U', $next['options'], $m);
  $fx_barsize = explode(',', $m[1]);
  foreach ($fx_barsize as $k=>$v)
  {
    $v = intval(trim($v));
    if (empty($v)) unset($fx_barsize[$k]);
    else $fx_barsize[$k] = $v;
  }
  preg_match('`futures\:(.+)\n`U', $next['options'], $m);
  $futures = explode(',', $m[1]);
  foreach ($futures as $k=>$v)
  {
    $v = intval(trim($v));
    if (empty($v)) unset($futures[$k]);
    else $futures[$k] = $v;
  }
  preg_match('`futures_barsize\:(.+)\n`U', $next['options'], $m);
  $futures_barsize = explode(',', $m[1]);
  foreach ($futures_barsize as $k=>$v)
  {
    $v = intval(trim($v));
    if (empty($v)) unset($futures_barsize[$k]);
    else $futures_barsize[$k] = $v;
  }
  echo '
    oldsubs.push(
      {
        fx: ['.implode(',', $fx).'],
        fx_barsize: ['.implode(',', $fx_barsize).'],
        futures: ['.implode(',', $futures).'],
        futures_barsize: ['.implode(',', $futures_barsize).']
      }
    );
  ';
}
?>
function in_array(needle, haystack, strict) {	// Checks if a value exists in an array
	// 
	// +   original by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)

	var found = false, key, strict = !!strict;

	for (key in haystack) {
		if ((strict && haystack[key] === needle) || (!strict && haystack[key] == needle)) {
			found = true;
			break;
		}
	}

	return found;
}

function Dump(d,l) {
    if (l == null) l = 1;
    var s = '';
    if (typeof(d) == "object") {
        s += typeof(d) + " {\n";
        for (var k in d) {
            for (var i=0; i<l; i++) s += "  ";
            s += k+": " + Dump(d[k],l+1);
        }
        for (var i=0; i<l-1; i++) s += "  ";
        s += "}\n"
    } else {
        s += "" + d + "\n";
    }
    return s;
}


function docalc()
{
    var month = $('#monthno').val();
    var fx_cost = 0;
    var futures_cost = 0;
    var equities_cost = 0;
    var etf_cost = 0;
    
    var fx_costs = [];
    $('#fx-tab .bs input:checked').each(function(i, bs_obj){
      for (var k in prices[1][$('#fx_data_provider_select').val()])
      {
        var cur = prices[1][$('#fx_data_provider_select').val()][k];
        if (cur.exchange == 1 && cur.barsize == $(bs_obj).val()) fx_costs.push(cur.price*1);
      }
    });
    
    $('#futures-tab .cp input:checked').each(function(j, cp_obj){
      $('#futures-tab .bs input:checked').each(function(i, bs_obj){
        for (var k in prices[2][$('#futures_data_provider_select').val()])
        {
          var cur = prices[2][$('#futures_data_provider_select').val()][k];
          if (cur.exchange == $(cp_obj).parent().parent().attr('exchange_id') && cur.barsize == $(bs_obj).val()) futures_cost += cur.price*1;
          
        }
      });
    });
    
    $('#equities-tab .cp input:checked').each(function(j, cp_obj){
      $('#equities-tab .bs input:checked').each(function(i, bs_obj){
        for (var k in prices[3][$('#equities_data_provider_select').val()])
        {
          var cur = prices[3][$('#equities_data_provider_select').val()][k];
          if (cur.exchange == $(cp_obj).parent().parent().attr('exchange_id') && cur.barsize == $(bs_obj).val()) equities_cost += cur.price*1;
        }
      });
    });
    
    $('#etf-tab .cp input:checked').each(function(j, cp_obj){
      $('#etf-tab .bs input:checked').each(function(i, bs_obj){
        for (var k in prices[4][$('#etf_data_provider_select').val()])
        {
          var cur = prices[4][$('#etf_data_provider_select').val()][k];
          if (cur.exchange == $(cp_obj).parent().parent().attr('exchange_id') && cur.barsize == $(bs_obj).val()) etf_cost += cur.price*1;
        }
      });
    });
    

    dataload = dlelement = document.getElementById("dataload");
    dataload = Math.abs(dataload.options[dataload.selectedIndex].value).toFixed(2);
    
    //d1 = new Date();
    //d2 = new Date();
    
    //d1.setTime(Date.parse($("#datepickerfromalt").val()));
    //d2.setTime(Date.parse($("#datepickertoalt").val()));
    
    /*
    if (curdate.getDate() == d2.getDate() && curdate.getMonth() == d2.getMonth() && curdate.getYear() == d2.getYear())
    {
      dlelement.options[1].disabled = false;
    }
    else
    {
      dlelement.selectedIndex = 0;
      dlelement.options[1].disabled = true;
      dataload = (0).toFixed(2);
    }
    */
    
    var gross = 0;
    var fx_gross = 0;
    var futures_gross = 0;
    
    //diff = +d2-d1;
    
    diff = $('#daterange_to').val()-$('#daterange_from').val()+1;
    if (diff < 0)
    {
      alert('You cannot chose "From Date" BEFORE "To Date"');
      //$("#datepickerfrom").datepicker("setDate",oldfrom);
      //$("#datepickerto").datepicker("setDate",oldto);
      return;
    }
    //diff = Math.floor((Math.abs(diff) / 1000 / 3600 / 24).toFixed(0));
    //diff = (diff/30).toFixed(0);
    
    
    var fx1 = 0;
    var fx2 = 0;
    if ($('#bs1').is(':checked'))
    {
      fx1 = 100 + ($('#fx-tab .cp input:checked').length * (50 / $('#fx-tab .cp').length / 12) * diff);
      if (fx1 < 100) fx1 = 100;
    }
    if ($('#bs2').is(':checked'))
    {
      fx2 = 200 + ($('#fx-tab .cp input:checked').length * (100 / $('#fx-tab .cp').length / 12) * diff);
      if (fx2 < 200) fx2 = 200;
    }
    
    if ($('#fx-tab .cp input:checked').length==0) fx_gross = 0;
    else if (fx1 > fx2) fx_gross = fx1;
    else fx_gross = fx2;
    
    var futures1 = 0;
    var futures2 = 0;
    if ($('#bs21').is(':checked'))
    {
      futures1 = 120 + ($('#futures-tab .cp input:checked').length * (6/12) * diff);
      if (futures1 < 120) futures1 = 120;
    }
    if ($('#bs22').is(':checked'))
    {
      futures2 = 240 + ($('#futures-tab .cp input:checked').length * (9/12) * diff);
      if (futures2 < 240) futures2 = 240;
    }
    
    if ($('#futures-tab .cp input:checked').length==0) futures_gross = 0;
    else if (futures1 > futures2) futures_gross = futures1;
    else futures_gross = futures2;
    
    //if (is_tester==0) futures_gross = 0;
    
    gross = fx_gross + futures_gross;
    
    gross = Math.abs(gross).toFixed(2);
    
    if ($('#us_economic_calendar').is(':checked')) gross = gross*1 + 25;
    //cost=Math.abs(0).toFixed(2);
    //cost = (500 + ((0.3 * fx_cost * diff + 0.3 * futures_cost * diff + 0.3 * equities_cost * diff + 0.3 * etf_cost * diff) / $('#fx-tab .cp').length)).toFixed(2);
    
    //availability = ($('#availability').val()=='24hours' ? 150 : 0).toFixed(2);
    
    //gross = Math.abs(Math.abs(cost) + Math.abs(dataload) + Math.abs(availability)).toFixed(2);
    
    
    
    var discount = Math.abs(0).toFixed(2);
    var discountp = 0;
    var discpromo = 0;
    <?php
    if (1==0 && preg_match('`\?promo\=(.+)$`', $_SERVER['REQUEST_URI'], $promo)) $_SESSION['promocode'] = @$promo[1];
    if (!empty($_SESSION['promocode']))
    {
      $next = mysql_fetch_assoc(mysql_query('select * from dcs_promocode where name="'.mysql_escape_string($_SESSION['promocode']).'" and status=1'));
      if (!empty($next['id'])) echo 'discpromo = '.intval($next['amount']).';';
    }
    ?>
    
    //var disc2 = disc.substr(4,2) * 1;
    
    if (disc2 > 0) discount = gross * (disc2 / 100);
    else if (discpromo != 0) discount = gross * (discpromo / 100);
    else
    {
      if (month == 3 || month == 4 || month == 5) discount = gross * 0.05;
      if (month == 6 || month == 7 || month == 8) discount = gross * 0.10;
      if (month == 9 || month == 10 || month == 11 || month == 12) discount = gross * 0.15;
    }
    netfee = gross - discount;
    total = netfee * month;
    
    if (gross > 0) discountp = discount / gross * 100;
    
    minusd = (discount > 0 ? '-' : '');
    //$('#fee1').html(cost);
    //$('#fee_availability').html(availability);
    //$('#fee2').html(dataload);
    //$('#fee3').html(gross);
    
    $('#fee4').html(minusd + Math.abs(discount).toFixed(2)  + ' ( ' + Math.abs(discountp).toFixed(0) +'% ) ');
    $('#fee5').html(Math.abs(netfee).toFixed(2));
    $('#fee6').html(month + (month == 1 ? ' Month' : ' Months'));
    $('#fee7').html(Math.abs(total).toFixed(2));
    
    $('#hfee1').val(0);
    $('#hfee2').val(dataload);
    $('#hfee3').val(gross);
    $('#hfee4').val(minusd + Math.abs(discount).toFixed(2));
    $('#hfee5').val(Math.abs(netfee).toFixed(2));
    $('#hfee6').val(Math.abs(total).toFixed(2));
    
    if (
      ($('#fx-tab .bs input:checked').length!=0 && $('#fx-tab .cp input:checked').length!=0)
      || ($('#futures-tab .bs input:checked').length!=0 && $('#futures-tab .cp input:checked').length!=0)
    ) $('#nextbutton').attr('disabled', false);
    else { $('#nextbutton').attr('disabled', true); }
    
    $('#futures_selected span').html($('#futures-tab .cp input:checked').length);
    $('#equities_selected span').html($('#equities-tab .cp input:checked').length);
}



var discount_checked = false;
var discount_success = false;
var submited = false;
function check_discount()
{
  discount_checked = false;
  if (discount_success || $('#promocode').val() == '')
  {
    discount_checked = true;
    return;
  }
  $('#discloading').css('display','inline');
  $('#promocode').attr('disabled', true);
  
  $.post('/check_discount.php', 
    {
      disc: $('#promocode').val(),
      type: 2
    },
    function(data)
    {
      disc2 = parseInt(data);
      $('#discloading').hide();
      if (disc2 == 0) $('#promocode').attr('disabled', false);
      else
      {
        docalc();
        discount_success = true;
      }
      discount_checked = true;
    }
  );
}

function is_checked_discount()
{
  if (discount_checked)
  {
    $('#promocode').attr('disabled', false);
    $('#formsubcr').submit();
  }
  else setTimeout(is_checked_discount, 1000)
}

</SCRIPT>
<div id='reg'><form method=post id="formsubcr" action='subscribe/' onsubmit="$('#datepickerto').attr('disabled',false); $('#nextbutton').attr('disabled', true); if (!submited) { submited = true; check_discount(); is_checked_discount(); return false; } ">
Please note that you must apply for DCS Trial access prior to subscribing for DCS.<br>Please use the form below to choose your DCS subscription options for historical market data and subscription length (i.e. how long you are signing up for).<br>A summary of the cost will be updated as you pick options.<br><br>
<input type=hidden name='hfee1' value=0 id='hfee1'><input type=hidden name='hfee2' value=0 id='hfee2'><input type=hidden name='hfee3' value=0 id='hfee3'><input type=hidden name='hfee4' value=0 id='hfee4'><input type=hidden name='hfee5' value=0 id='hfee5'><input type=hidden name='hfee6' value=0 id='hfee6'>

<style>
table.options {background: #fff; width:100%}
table.options td {padding:3px 5px ; background:#ddd}
</style>


<style>
.ui-tabs-nav li.first {
  margin-top:7px;
  font-weight:normal;
}
</style>

<?php
  $sth = mysql_query('select * from data_providers where 1 order by ordering asc') or die(mysql_error());
  $providers = array();
  while ($next = mysql_fetch_assoc($sth)) $providers[] = $next;
?>
<div id="tabs">
	<ul>
    <li class=first>&nbsp; <b>Market Data:</b>&nbsp;</li>
		<li><a href="#fx-tab">FX</a></li>
		<li><a href="#futures-tab">Futures</a></li>
    
		<li<?php if (intval($userinfo['is_tester'])<=1) echo ' style="display:none"';?>><a href="#equities-tab">Equities</a></li>
    <!--
    <li><a href="#etf-tab">ETF</a></li>
    -->
    <li><a href="#addons-tab">Supplemental</a></li>
	</ul>
  
	<div id="fx-tab">
  
    <div style="display:none;">
    <table style="margin:0 0 6px 10px">
      <div class=tabheading style="display:inline"><div>Data Provider</div>:</div>&nbsp;
      <select id="fx_data_provider_select" name="fx_provider">
      <?php
      $sth = mysql_query('
        select data_providers.*
        from data_providers
          inner join data_limitdates on data_limitdates.provider_id = data_providers.id
          inner join data_products on data_limitdates.product_id = data_products.id
        where data_products.type_id = 1
        group by data_providers.id
        order by data_providers.ordering asc
      ') or die(mysql_error());
      while ($next = mysql_fetch_assoc($sth)) echo '<option value="'.$next['id'].'">'.$next['name'];
      ?>
      </select>
    </table>
  </div>
    <?php
    //echo '<pre>';print_r($subinfo);echo '</pre>';
    ?>
    <table cellspacing=1 class=options>
    <tr><td><span style="font-size: 9pt; font-weight: bold">Bar Size:</span>
    <?php
      $sth = mysql_query('select * from data_barsizes where 1 order by ordering asc');
      while ($ro = mysql_fetch_assoc($sth)) $bars[] = $ro;
      
      foreach ($bars as $ro)
        echo '
          <span class=bs style="display:inline">
          <input type=checkbox id="bs'.$ro['id'].'" value="'.$ro['id'].'" name="fx_bs['.$ro['id'].']"'.(in_array($ro['id'],$subinfo['fx_barsize'])?' checked':'').'>
          <label for="bs'.$ro['id'].'">'.$ro['bar_size_text'].'</label> &nbsp; </span>
        ';
    ?>
    <tr><td>
    <span style="font-size: 9pt; font-weight: bold">Select Currency Pair(s):</span>
    <a class=clearall href="#" onclick="if ($(this).text()=='Clear all') {$('#fx-tab .cp input:visible').attr('checked', false);$(this).text('Select all');} else {$('#fx-tab .cp input:visible').attr('checked', true);$(this).text('Clear all');} return false;">Select all</a>    
    <br>
    <?         
     $re = mysql_query("select * FROM data_products where type_id = 1 ORDER BY ordering asc");
     if ($re && mysql_num_rows($re))
     {
       while ($ro = mysql_fetch_array($re))
        echo '
          <div class="cp" product_id="'.$ro['id'].'" style="display:none">
          <input type="checkbox" id="cb'.$ro['id'].'" name="fx['.$ro['id'].']" value="'.$ro['id'].'">
          <label for="cb'.$ro['id'].'"> '.$ro['symbol'].'</label>
          </div>
        ';
     }
    ?>

    </table>
  </div>
  
  <div id="futures-tab">
    <div style="display:none;">
    <div class=tabheading style="display:inline"><div>Data Provider</div>:</div>&nbsp;
    <select id="futures_data_provider_select" name="futures_provider" multiaple="multiple">
    <?php
    $sth = mysql_query('
      select data_providers.*
      from data_providers
        inner join data_limitdates on data_limitdates.provider_id = data_providers.id
        inner join data_products on data_limitdates.product_id = data_products.id
      where data_products.type_id = 2
      group by data_providers.id
      order by data_providers.ordering asc
    ') or die(mysql_error());
    while ($next = mysql_fetch_assoc($sth)) echo '<option value="'.$next['id'].'">'.$next['name'];
    ?>
    </select>
    <div style="height:6px;clear:both"></div>
    </div>
    
    <input type="checkbox" name="futures_type[]" id="futures_type_raw" value="raw" disabled><label for="futures_type_raw"> Raw Futures</label> &nbsp; <input type="checkbox" name="futures_type[]" id="futures_type_ñontinuous" value="continuous" checked disabled><label for="futures_type_ñontinuous"> Continuous Futures</label>
	
    
    <div class=tabheading><div>Regions</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#futures_region_div input').attr('checked', false);$(this).text('Select all');} else {$('#futures_region_div input').attr('checked', true);$(this).text('Clear all');} on_region_changed_all(); show_futures(); return false;">Clear all</a></div>
    <div id="futures_region_div" class=parsdiv>
    <?php
    $sth = mysql_query('select * from data_regions where 1 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth))
    {
      echo '<div class=pars region_id="'.$next['id'].'"><input type=checkbox id="regioncheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="regioncheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    }
    ?>
    </div>
    <div style="height:6px;clear:both"></div>

    
    <div class=tabheading><div>Markets</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#futures_data_market_div input').attr('checked', false);$(this).text('Select all');} else {$('#futures_data_market_div input').attr('checked', true);$(this).text('Clear all');} show_futures(); return false;">Clear all</a></div>
    <div class=parsdiv id="futures_data_market_div">
    <?php
    $sth = mysql_query('
      select data_exchanges.*
      from data_exchanges
        inner join data_limitdates on data_limitdates.exchange_id = data_exchanges.id
        inner join data_products on data_limitdates.product_id = data_products.id
      where data_products.type_id = 2
      group by data_exchanges.id
      order by data_exchanges.ordering asc
    ');
    while ($next = mysql_fetch_assoc($sth))
    {
      echo '<div class=pars exchange_id="'.$next['id'].'" region_id="'.$next['region_id'].'"><input type=checkbox id="exchangecheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="exchangecheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    }
    ?>
    </div>
    <div style="height:6px;clear:both"></div>
    
    <div class=tabheading><div>Categories</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#futures_cat_div input').attr('checked', false);$(this).text('Select all');} else {$('#futures_cat_div input').attr('checked', true);$(this).text('Clear all');} show_futures(); return false;">Clear all</a></div>
    <div class=parsdiv id="futures_cat_div">
    <?php
    $sth = mysql_query('select * from data_categories where type_id=2 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth)) echo '<div class=pars cat_id="'.$next['id'].'"><input type=checkbox id="futurecatcheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="futurecatcheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    ?>
    </div>
    <div style="height:6px;clear:both"></div>


    <table cellspacing=1 class=options>
    <tr><td style="background:#F6DBDB;border:1px solid #EEA8A8"><span style="font-weight: bold;text-decoaration:underline">Bar Size:</span><div style="float:right;color:red;font-size:80%;font-weight:bold">* Mandatory to check one or more! &nbsp; &nbsp;</div>
    <? foreach ($bars as $ro) echo "<span class='bs' style='font-size:100%;display:inline'><input type=checkbox id='bs2".$ro[id]."' value=$ro[id] name='futures_bs[$ro[id]]'><label for='bs2".$ro[id]."'>$ro[bar_size_text]</label> &nbsp; </span>\n"; ?>

    <tr><td align=right><div style="font-weight:bold; font-size:90%;" id="futures_selected">Selected items: <span>0</span>&nbsp; &nbsp; </div><div style="font-weight:bold; font-size:90%; display:none" id="futures_found">Matched items: <span>0</span>&nbsp; &nbsp; </div></td></tr>
    <tr><td valign=top>
    <div id=futures style="height:400px; overflow:auto">
    
    <div id="futures_loading" style="display:none;text-align:center"><img src="loading45.gif" alt="" border=0></div>
    
    
    <table id="futures_table" class="tablesorter">
    <thead>
    <tr>
    <th><input type=checkbox onclick="if ($(this).is(':checked')) $('#futures-tab .cp input:visible:enabled').attr('checked', true); else $('#futures-tab .cp input:visible').attr('checked', false); docalc()"></th>
    <th>Region</th>
    <th>Exchange</th>
    <th>Category</th>
    <th>Currency Futures</th>
    <th>Symbol</th>
    </tr>
    </thead>
    <tbody>
    </tbody>
    </table>
    </div>
    </td>
    </table>
  </div>
  
  <div id="equities-tab"<?php if (intval($userinfo['is_tester'])<=1) echo ' style="display:none"';?>>
  
  
  <div style="display:none;">
    <div class=tabheading style="display:inline"><div>Data Provider</div>:</div>&nbsp;
    <select id="equities_data_provider_select" name="equities_provider" multiaple="multiple">
    <?php
    $sth = mysql_query('
      select data_providers.*
      from data_providers
        inner join data_limitdates on data_limitdates.provider_id = data_providers.id
        inner join data_products on data_limitdates.product_id = data_products.id
      where data_products.type_id = 3
      group by data_providers.id
      order by data_providers.ordering asc
    ') or die(mysql_error());
    while ($next = mysql_fetch_assoc($sth)) echo '<option value="'.$next['id'].'">'.$next['name'];
    ?>
    </select>
    <div style="height:6px;clear:both"></div>
  </div>
  
<!--
    <div class=tabheading style="display:inline">
    <div>Index Membership</div>:
    <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#equities_index_div input').attr('checked', false);$(this).text('Select all');} else {$('#equities_index_div input').attr('checked', true);$(this).text('Clear all');} show_equities(); return false;">Clear all</a>
    </div>
    &nbsp; &nbsp; Date Period: <input type=text id=index_from value="01.01.1999" style="font-size:90%;width:90px"> &ndash; <input type=text id=index_to value="01.01.2013" style="font-size:90%;width:90px">
    <div id="equities_index_div" class=parsdiv>
    <?php
    $sth = mysql_query('
      select data_indexes.*
      from data_indexes
      where 1
      order by ordering asc
    ');
    while ($next = mysql_fetch_assoc($sth))
    {
      echo '<div class=pars index_id="'.$next['id'].'"><input type=checkbox id="equities_indexcheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="equities_indexcheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    }
    ?>
    </div>
    <div style="height:6px;clear:both"></div>
    

    <div class=tabheading><div>Regions</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#equities_region_div input').attr('checked', false);$(this).text('Select all');} else {$('#equities_region_div input').attr('checked', true);$(this).text('Clear all');} on_region_changed_all2(); show_equities(); return false;">Clear all</a></div>
    <div id="equities_region_div" class=parsdiv>
    <?php
    $sth = mysql_query('select * from data_regions where 1 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth))
    {
      echo '<div class=pars region_id="'.$next['id'].'"><input type=checkbox id="equitiesregioncheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="equitiesregioncheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    }
    ?>
    </div>
    <div style="height:6px;clear:both"></div>
-->
    
    <div class=tabheading><div>Markets</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#equities_data_market_div input').attr('checked', false);$(this).text('Select all');} else {$('#equities_data_market_div input').attr('checked', true);$(this).text('Clear all');} show_equities(); return false;">Clear all</a></div>
    <div class=parsdiv id="equities_data_market_div">
    <?php
    $sth = mysql_query('
      select data_exchanges.*
      from data_exchanges
        inner join data_limitdates on data_limitdates.exchange_id = data_exchanges.id
        inner join data_products on data_limitdates.product_id = data_products.id
      where data_products.type_id = 3
      group by data_exchanges.id
      order by data_exchanges.ordering asc
    ');
    while ($next = mysql_fetch_assoc($sth))
    {
      echo '<div class=pars exchange_id="'.$next['id'].'" region_id="'.$next['region_id'].'"><input type=checkbox id="equities_exchangecheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="equities_exchangecheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    }
    ?>
    </div>
    <div style="height:6px;clear:both"></div>

    <div class=tabheading><div>Security types</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#equities_sectype_div input').attr('checked', false);$(this).text('Select all');} else {$('#equities_sectype_div input').attr('checked', true);$(this).text('Clear all');} show_equities(); return false;">Clear all</a></div>
    <div class=parsdiv id="equities_sectype_div">
    <?php
    $sth = mysql_query('select * from data_secutirytypes where 1 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth)) echo '<div class=pars sectype_id="'.$next['id'].'"><input type=checkbox id="equitiesseccheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="equitiesseccheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    ?>
    </div>
    <div style="height:6px;clear:both"></div>
    
    <div class=tabheading><div>NAICS Sector</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#equities_cat_div input').attr('checked', false);$(this).text('Select all');} else {$('#equities_cat_div input').attr('checked', true);$(this).text('Clear all');} show_equities(); return false;">Clear all</a></div>
    <div class=parsdiv id="equities_cat_div">
    <?php
    $sth = mysql_query('select * from data_categories where type_id=3 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth)) echo '<div class=pars cat_id="'.$next['id'].'"><input type=checkbox id="equitiescatcheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="equitiescatcheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    ?>
    </div>
    <div style="height:6px;clear:both"></div>


    <table cellspacing=1 class=options>
    <tr><td style="background:#F6DBDB;border:1px solid #EEA8A8"><span style="font-weight: bold;text-decoaration:underline">Bar Size:</span><div style="float:right;color:red;font-size:80%;font-weight:bold">* Mandatory to check one or more! &nbsp; &nbsp;</div>
    <? foreach ($bars as $ro) echo "<span class='bs' style='font-size:100%;display:inline'><input type=checkbox id='bs3".$ro[id]."' value=$ro[id] name='equities_bs[$ro[id]]'><label for='bs3".$ro[id]."'>$ro[bar_size_text]</label> &nbsp; </span>\n"; ?>
    
    <tr><td align=right><div style="font-weight:bold; font-size:90%;" id="equities_selected">Selected items: <span>0</span>&nbsp; &nbsp; </div><div style="font-weight:bold; font-size:90%; disaplay:none" id="equities_found">Matched items: <span>0</span>&nbsp; &nbsp; </div></td></tr>
    <tr><td valign=top>
    <div id=equities style="height:500px; overflow:auto">
    
    <table id="equities_table" class="tablesorter">
    <thead>
    <tr>
    <th><input type=checkbox onclick="if ($(this).is(':checked')) $('#equities-tab .cp input:visible').attr('checked', true); else $('#equities-tab .cp input:visible').attr('checked', false); docalc()"></th>
    <th>CUSIP</th>
    <th>Symbol</th>
    <th>Exchange</th>
    <th>Company Name</th>
    <th>Security Type</th>
    <th>Issue Status</th>
    <th>Sector</th>
    </tr>
    </thead>
    <tbody>
 
    </tbody>
    </table>
    <div id="equities_loading" style="display:none;text-align:center;margin:10px 0"><a href="#" onclick="get_equities(1);$('#equities_loading').hide();return false" title="Click to show more matches"><img src="loading45.gif" alt="" border=0></a></div>
    </div>
    </td>
    </table>
  
  
  
  
  </div>
  
  <div id="etf-tab" style="display:none">
  
  
  
  <div style="display:none;">
    <div class=tabheading style="display:inline"><div>Data Provider</div>:</div>&nbsp;
    <select id="etf_data_provider_select" name="etf_provider" multiaple="multiple">
    <?php
    $sth = mysql_query('
      select data_providers.*
      from data_providers
        inner join data_limitdates on data_limitdates.provider_id = data_providers.id
        inner join data_products on data_limitdates.product_id = data_products.id
      where data_products.type_id = 4
      group by data_providers.id
      order by data_providers.ordering asc
    ') or die(mysql_error());
    while ($next = mysql_fetch_assoc($sth)) echo '<option value="'.$next['id'].'">'.$next['name'];
    ?>
    </select>
    <div style="height:6px;clear:both"></div>
  </div>
    <!--
    <div class=tabheading><div>Regions</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#etf_region_div input').attr('checked', false);$(this).text('Select all');} else {$('#etf_region_div input').attr('checked', true);$(this).text('Clear all');} on_region_changed_all3(); show_etf(); return false;">Clear all</a></div>
    <div id="etf_region_div" class=parsdiv>
    <?php
    $sth = mysql_query('select * from data_regions where 1 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth))
    {
      echo '<div class=pars region_id="'.$next['id'].'"><input type=checkbox id="etfregioncheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="etfregioncheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    }
    ?>
    </div>
    <div style="height:6px;clear:both"></div>
-->
    
    <div class=tabheading><div>Markets</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#etf_data_market_div input').attr('checked', false);$(this).text('Select all');} else {$('#etf_data_market_div input').attr('checked', true);$(this).text('Clear all');} show_etf(); return false;">Clear all</a></div>
    <div class=parsdiv id="etf_data_market_div">
    <?php
    $sth = mysql_query('
      select data_exchanges.*
      from data_exchanges
        inner join data_limitdates on data_limitdates.exchange_id = data_exchanges.id
        inner join data_products on data_limitdates.product_id = data_products.id
      where data_products.type_id = 4
      group by data_exchanges.id
      order by data_exchanges.ordering asc
    ');
    while ($next = mysql_fetch_assoc($sth))
    {
      echo '<div class=pars exchange_id="'.$next['id'].'" region_id="'.$next['region_id'].'"><input type=checkbox id="etf_exchangecheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="etf_exchangecheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    }
    ?>
    </div>
    <div style="height:6px;clear:both"></div>

    <div class=tabheading><div>Asset Class</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#etf_assetclass_div input').attr('checked', false);$(this).text('Select all');} else {$('#etf_assetclass_div input').attr('checked', true);$(this).text('Clear all');} show_etf(); return false;">Clear all</a></div>
    <div class=parsdiv id="etf_assetclass_div">
    <?php
    $sth = mysql_query('select * from data_asset_classes where 1 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth)) echo '<div class=pars asset_class_id="'.$next['id'].'"><input type=checkbox id="etfassetclasscheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="etfassetclasscheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    ?>
    </div>
    <div style="height:6px;clear:both"></div>
    
    <div class=tabheading><div>Asset Class Size</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#etf_assetsize_div input').attr('checked', false);$(this).text('Select all');} else {$('#etf_assetsize_div input').attr('checked', true);$(this).text('Clear all');} show_etf(); return false;">Clear all</a></div>
    <div class=parsdiv id="etf_assetsize_div">
    <?php
    $sth = mysql_query('select * from data_asset_class_sizes where 1 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth)) echo '<div class=pars asset_class_size_id="'.$next['id'].'"><input type=checkbox id="etfassetclasssizecheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="etfassetclasssizecheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    ?>
    </div>
    <div style="height:6px;clear:both"></div>
    
    <div class=tabheading><div>Asset Class Style</div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#etf_assetstyle_div input').attr('checked', false);$(this).text('Select all');} else {$('#etf_assetstyle_div input').attr('checked', true);$(this).text('Clear all');} show_etf(); return false;">Clear all</a></div>
    <div class=parsdiv id="etf_assetstyle_div">
    <?php
    $sth = mysql_query('select * from data_asset_class_styles where 1 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth)) echo '<div class=pars asset_class_style_id="'.$next['id'].'"><input type=checkbox id="etfassetclassstylecheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="etfassetclassstylecheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    ?>
    </div>
    <div style="height:6px;clear:both"></div>
    
    <div class=tabheading><div title="Click to expand" style="cursor:pointer;" onclick="if ($('#etf_cat_div').is(':visible')) {$('#etf_cat_div').slideUp();$(this).children('img').attr('src','Collapsed.gif');} else {$('#etf_cat_div').slideDown();$(this).children('img').attr('src','Expanded.gif');}"><img src="Collapsed.gif" border=0 alt=""> <span style="border-bottom:1px dashed black">Sector</span></div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#etf_cat_div input').attr('checked', false);$(this).text('Select all');} else {$('#etf_cat_div input').attr('checked', true);$(this).text('Clear all');} show_etf(); return false;">Clear all</a></div>
    <div class=parsdiv id="etf_cat_div" style="display:none">
    <?php
    $sth = mysql_query('select * from data_categories where type_id=4 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth)) echo '<div class=pars cat_id="'.$next['id'].'"><input type=checkbox id="etfcatcheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="etfcatcheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    ?>
    </div>
    <div style="height:6px;clear:both"></div>
    
    <div class=tabheading><div title="Click to expand" style="cursor:pointer;" onclick="if ($('#etf_issuer_div').is(':visible')) {$('#etf_issuer_div').slideUp();$(this).children('img').attr('src','Collapsed.gif');} else {$('#etf_issuer_div').slideDown();$(this).children('img').attr('src','Expanded.gif');}"><img src="Collapsed.gif" border=0 alt=""> <span style="border-bottom:1px dashed black">Issuer</span></div>: <a href="#" class=clearall onclick="if ($(this).text()=='Clear all') {$('#etf_issuer_div input').attr('checked', false);$(this).text('Select all');} else {$('#etf_issuer_div input').attr('checked', true);$(this).text('Clear all');} show_etf(); return false;">Clear all</a></div>
    <div class=parsdiv id="etf_issuer_div" style="display:none">
    <?php
    $sth = mysql_query('select * from data_issuers where 1 order by ordering asc');
    while ($next = mysql_fetch_assoc($sth)) echo '<div class=pars issuer_id="'.$next['id'].'"><input type=checkbox id="etfissuercheckbox'.$next['id'].'" value="'.$next['id'].'" checked><label for="etfissuercheckbox'.$next['id'].'"> '.$next['name'].'</label></div>';
    ?>
    </div>
    <div style="height:6px;clear:both"></div>
    
    
    <div class=parsdiv id="etf_leveraged_div">
    <div style="float: left;margin-right:40px;">&nbsp; &nbsp; Underlying Index: <input type=text id="etf_und_index" style="width:200px"></div>
    <div class=pars><input type=checkbox id="etf_leveraged_on" checked><label for=etf_leveraged_on> Leveraged</label></div>
    <div class=pars><input type=checkbox id="etf_leveraged_off" checked><label for=etf_leveraged_off> Non Leveraged</label></div>
    </div>
    <div style="height:6px;clear:both"></div>
    
    <table cellspacing=1 class=options>
    <tr><td style="background:#F6DBDB;border:1px solid #EEA8A8"><span style="font-weight: bold;text-decoaration:underline">Bar Size:</span><div style="float:right;color:red;font-size:80%;font-weight:bold">* Mandatory to check one or more! &nbsp; &nbsp;</div>
    <? foreach ($bars as $ro) echo "<span class='bs' style='font-size:100%;display:inline'><input type=checkbox id='bs4".$ro[id]."' value=$ro[id] name='etf_bs[$ro[id]]'><label for='bs4".$ro[id]."'>$ro[bar_size_text]</label> &nbsp; </span>\n"; ?>

    <tr><td align=right><div style="font-weight:bold; font-size:90%" id="etf_found">Matched items: <span>0</span>&nbsp; &nbsp; </div></td></tr>
    <tr><td valign=top>
    <div id=etf style="height:500px; overflow:auto">
    
    <table id="etf_table" class="tablesorter">
    <thead>
    <tr>
    <th><input type=checkbox onclick="if ($(this).is(':checked')) $('#etf-tab .cp input:visible').attr('checked', true); else $('#etf-tab .cp input:visible').attr('checked', false); docalc()"></th>
    <th>Symbol</th>
    <th>Exchange</th>
    <th>Company Name</th>
    <th>Underlying Index</th>
    <th>Sector</th>
    <th>Issuer</th>
    <th>Asset Class</th>
    <th>Asset Class Size</th>
    <th>Asset Class Style</th>
    </tr>
    </thead>
    <tbody>

    </tbody>
    </table>
    <div id="etf_loading" style="display:none;text-align:center;margin:10px 0"><a href="#" onclick="get_etf(1);$('#etf_loading').hide();return false" title="Click to show more matches"><img src="loading45.gif" alt="" border=0></a></div>
    </div>
    </td>
    </table>
  
  
  
  
  </div>
  
  <div id="addons-tab">
    <table id="addons_table" class="tablesorter">
    <thead>
    <tr>
    <th></th>
    <th>Data Vendor</th>
    <th>Definition</th>
    <th>Applicable Markets</th>
    <th>Price</th>
    </tr>
    </thead>
    <tbody>
    <tr class="cp" style="display:table-row;">
    <td><input type=checkbox name="us_economic_calendar" id="us_economic_calendar" value="1"></td>
    <td>Econoday, Inc</td>
    <td>World Economic Calendar</td>
    <td>{FX, Futures, Equities}</td>
    <td>$25.00 per month</td>
    </tr>
    </tbody>
    </table>
    

  </div>
</div>


<script>
	$('#etf').scroll(function(){
    if ($('#etf').scrollTop() >= $('#etf_table').height() - $('#etf').height()){
      get_etf(1);
    }
  });
	$('#equities').scroll(function(){
    if ($('#equities').scrollTop() >= $('#equities_table').height() - $('#equities').height()){
      get_equities(1);
    }
  });

  $('#tabs').tabs();
  
  $('body').on("click", '.bs input', docalc);
  $('body').on("click", '.dateperiod', docalc);
  $('body').on("click", '#us_economic_calendar', docalc);
  $('body').on("click", '.cp input', docalc);
  $('body').on("click", '.clearall', docalc);
  
  
  
  
  $('#index_from').datepicker({
    dateFormat: "dd.mm.yy"
  });
  
  $('#index_to').datepicker({
    dateFormat: "dd.mm.yy"
  });
  
  var etf_limit = 0;
  var equities_limit = 0;
  
  $('#index_from').change(show_equities);
  $('#index_to').change(show_equities);
  
  $('#futures_region_div .pars input').click(on_region_changed);
  $('#futures-tab .pars input').click(show_futures);
  
  $('#equities_region_div .pars input').click(on_region_changed2);
  $('#equities-tab .pars input').click(show_equities);
  
  $('#etf_region_div .pars input').click(on_region_changed3);
  $('#etf-tab .pars input').click(show_etf);
  $('#etf_und_index').blur(show_etf);
  $('#etf_und_index').keypress(function(e){
    if (e.keyCode==13)
    {
      show_etf();
      e.preventDefault();
    }
  });

  
  
  var prices = [];
  <?php
  $sth = mysql_query('select * from data_types where 1');
  while ($next = mysql_fetch_assoc($sth))
  {
    echo 'prices['.$next['id'].'] = [];'."\r\n";
    foreach ($providers as $v) echo 'prices['.$next['id'].']['.$v['id'].'] = [];'."\r\n";
  }
  
  $sth = mysql_query('select * from data_prices where 1 order by id desc');
  while ($next = mysql_fetch_assoc($sth))
  {
    if (empty($prices[$next['type_id']][$next['provider_id']][$next['exchange_id']][$next['barsize_id']]))
    {
      $prices[$next['type_id']][$next['provider_id']][$next['exchange_id']][$next['barsize_id']] = $next['price'];
      echo 'prices['.$next['type_id'].']['.$next['provider_id'].'].push({exchange: '.$next['exchange_id'].', barsize: '.$next['barsize_id'].', price: '.$next['price'].'});'."\r\n";
    }
  }
  ?>
  
  function fx_data_provider_select_change()
  {
    
    $('#fx-tab .cp input').attr('checked', false);
    if ($('#fx_data_provider_select').val() == 0) $('.cp').show();
    else
    {
      $('#fx-tab .cp').hide();
      $.post('get_filters.php',
        {
          type: 1,
          what: 'product',
          provider: $('#fx_data_provider_select').val()
        },
        function(data){
          for (var i in data)
          {
            $('#fx-tab .cp[product_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
    }
    docalc();
  }
  
  $('#fx_data_provider_select').change(fx_data_provider_select_change);
  
  
  
  
  
  
  function futures_data_provider_select_change()
  {
    $('#futures-tab .cp input').attr('checked', false);
    var provider = $('#futures_data_provider_select').val();
    if (provider == 0) $('#futures-tab .cp').show();
    else
    {
      $('#futures-tab .pars').hide();
      
      $.post('get_filters.php',
        {
          type: 2,
          provider: provider,
          what: 'exchange'
        },
        function(data){
          for (var i in data)
          {
            $('#futures_data_market_div .pars[exchange_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 2,
          provider: provider,
          what: 'region'
        },
        function(data){
          for (var i in data)
          {
            $('#futures_region_div .pars[region_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 2,
          provider: provider,
          what: 'category'
        },
        function(data){
          for (var i in data)
          {
            $('#futures_cat_div .pars[cat_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
    
      var intervalID = setInterval(function() {
        if (
          $('#futures-tab .pars[exchange_id] input:visible').length > 0
          && $('#futures-tab .pars[cat_id] input:visible').length > 0
          && $('#futures-tab .pars[region_id] input:visible').length > 0
        )
        {
          clearInterval(intervalID);
          $('#futures-tab .pars input:visible').attr('checked', true);
          show_futures();
        }
      }, 100);
      
    }
  }
  
  $('#futures_data_provider_select').change(futures_data_provider_select_change);
  
  futures_load = 0;
  prev_checked = [];
  
  function show_futures()
  {
    if (prev_checked.length == 0)
    {
      $('#futures-tab .cp input:checked:visible').each(function(){
        prev_checked.push($(this).val());
      });
    }
    $('#futures_table tbody').empty();
    $('#futures_table').hide();
    $('#futures_loading').show();
    var provider = $('#futures_data_provider_select').val();
    var exchanges = [];
    $('#futures-tab .pars[exchange_id] input:checked:visible').each(function(){
      exchanges.push($(this).val());
    });
    var cats = [];
    $('#futures-tab .pars[cat_id] input:checked:visible').each(function(){
      cats.push($(this).val());
    });
    
    futures_load++;
    
    $.post('get_futures.php',
      {
        'exchanges[]': exchanges,
        'cats[]': cats,
        provider: provider
      },
      function(data){
        if (--futures_load > 0) return;
        
        $('#futures_loading').hide();
        for (var i in data.res)
        {
          $('#futures_table tbody').append('<tr class="cp" exchange_id="'+data.res[i].exchange_id+'" style="display:table-row;">\
<td><input type=checkbox name=futures[] value="'+data.res[i].id+'"'+(data.res[i].available=='0'?' disabled':'')+(in_array(data.res[i].id,prev_checked)?' checked':'')+'></td>\
<td'+(data.res[i].available=='0'?' style="color:#888"':'')+'>'+data.res[i].region+'</td>\
<td'+(data.res[i].available=='0'?' style="color:#888"':'')+'>'+data.res[i].exchange+'</td>\
<td'+(data.res[i].available=='0'?' style="color:#888"':'')+'>'+data.res[i].category+'</td>\
<td'+(data.res[i].available=='0'?' style="color:#888"':'')+'>'+data.res[i].name+'</td>\
<td'+(data.res[i].available=='0'?' style="color:#888"':'')+'>'+data.res[i].symbol+'</td>\
</tr>');

        }
        
        $('#futures_found span').html(data.count);
        $('#futures_table').show();
        $('#futures_table').tablesorter({ headers: {0: { sorter: false}, 4: { sorter: false}, 5: { sorter: false}, 6: { sorter: false}} });
        prev_checked = [];
      },
      'json'
    );
    
    docalc();
  }
  
  function on_region_changed(t)
  {
    //$('#futures_data_market_div input').attr('checked', false);
    $('#futures_data_market_div .pars[region_id='+$(this).val()+'] input').attr('checked', $(this).is(':checked'));
  }
  
  function on_region_changed_all()
  {
    $('#futures_data_market_div input').attr('checked', false);
    $('#futures_region_div .pars input:checked').each(function(){
      $('#futures_data_market_div .pars[region_id='+$(this).val()+'] input').attr('checked', true);
    });
  }
  
  
function array_unique( array ) {	// Removes duplicate values from an array
	// 
	// +   original by: Carlos R. L. Rodrigues

	var p, i, j;
	for(i = array.length; i;){
		for(p = --i; p > 0;){
			if(array[i] === array[--p]){
				for(j = p; --p && array[i] === array[p];);
				i -= array.splice(p + 1, j - p).length;
			}
		}
	}

	return true;
}

  
  
  function equities_data_provider_select_change()
  {
    var exchanges = [];
    var indexes = [];
    var regions = [];
    var cats = [];
    $('#equities-tab .cp input').attr('checked', false);
    $('#equities-tab .pars input').attr('checked', false);
    var provider = $('#equities_data_provider_select').val();
    if (provider == 0) $('#equities-tab .cp').show();
    else
    {
      $('#equities-tab .pars').hide();
      $('#equities_index_div .pars').show();
      
      $.post('get_filters.php',
        {
          type: 3,
          provider: provider,
          what: 'exchange'
        },
        function(data){
          for (var i in data)
          {
            $('#equities_data_market_div .pars[exchange_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 3,
          provider: provider,
          what: 'region'
        },
        function(data){
          for (var i in data)
          {
            $('#equities_region_div .pars[region_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 3,
          provider: provider,
          what: 'sectype'
        },
        function(data){
          for (var i in data)
          {
            $('#equities_sectype_div .pars[sectype_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 3,
          provider: provider,
          what: 'category'
        },
        function(data){
          for (var i in data)
          {
            $('#equities_cat_div .pars[cat_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );

      var intervalID = setInterval(function() {
        if (
          $('#equities-tab .pars[exchange_id] input:visible').length > 0
          && $('#equities-tab .pars[cat_id] input:visible').length > 0
          && $('#equities-tab .pars[sectype_id] input:visible').length > 0
          && (1==1||$('#equities-tab .pars[index_id] input:visible').length > 0)
        )
        {
          clearInterval(intervalID);
          $('#equities-tab .pars input:visible').attr('checked', true);
          show_equities();
        }
      }, 100);

    }
  }
  
  
  $('#equities_data_provider_select').change(equities_data_provider_select_change);
  
  
  equities_load = 0;
  function get_equities()
  {
    $('#equities_loading').show();
    var provider = $('#equities_data_provider_select').val();
    var exchanges = [];
    $('#equities-tab .pars[exchange_id] input:checked:visible').each(function(){
      exchanges.push($(this).val());
    });
    var sectypes = [];
    $('#equities-tab .pars[sectype_id] input:checked:visible').each(function(){
      sectypes.push($(this).val());
    });
    var indexes = [];
    $('#equities-tab .pars[index_id] input:checked:visible').each(function(){
      indexes.push($(this).val());
    });
    var cats = [];
    $('#equities-tab .pars[cat_id] input:checked:visible').each(function(){
      cats.push($(this).val());
    });
    
    if (arguments[0]!=1) equities_limit = 0;
    
    equities_load++;
    $.post('get_equities.php',
      {
        'exchanges[]': exchanges,
        'sectypes[]': sectypes,
        'indexes[]': indexes,
        date_from: $('#index_from').val(),
        date_to: $('#index_to').val(),
        'cats[]': cats,
        provider: provider,
        limit: equities_limit
      },
      
      function(data){
        if (--equities_load > 0) return;
        $('#equities_loading').hide();
        
        for (var i in data.res)
        {
          $('#equities_table tbody').append('<tr class="cp" exchange_id="'+data.res[i].exchange_id+'" style="display:table-row">\
<td><input type=checkbox name=equities[] value="'+data.res[i].id+'-'+data.res[i].exchange_id+'"></td>\
<td>'+data.res[i].cusip+'</td>\
<td>'+data.res[i].symbol+'</td>\
<td>'+data.res[i].exchange+'</td>\
<td>'+data.res[i].name+'</td>\
<td>'+data.res[i].security_type+'</td>\
<td>'+data.res[i].issue_status+'</td>\
<td>'+data.res[i].sector+'</td>\
</tr>');

        }
        $('#equities_found span').html(data.count);
        equities_limit += 100;
        equities_ajax = 0;
        $('#equities_table').show();
        //$('#equities_table').tablesorter({ headers: {0: { sorter: false}, 2: { sorter: false}, 4: { sorter: false}, 5: { sorter: false}} });
      },
      'json'
    );
  }
  
  function show_equities()
  {
    $('#equities_table tbody').empty();
    $('#equities_table').hide();
    get_equities();
    
    docalc();
    return true;

  }
  
  function on_region_changed2(t)
  {
    //$('#futures_data_market_div input').attr('checked', false);
    $('#equities_data_market_div .pars[region_id='+$(this).val()+'] input').attr('checked', $(this).is(':checked'));
  }
  
  function on_region_changed_all2()
  {
    $('#equities_data_market_div input').attr('checked', false);
    $('#equities_region_div .pars input:checked').each(function(){
      $('#equities_data_market_div .pars[region_id='+$(this).val()+'] input').attr('checked', true);
    });
  }
  
  
  
  
  function etf_data_provider_select_change()
  {
    
    var exchanges = [];
    var assets = [];
    var assetsizes = [];
    var assetstyles = [];
    var regions = [];
    var issuers = [];
    var cats = [];
    
    $('#etf-tab .cp input').attr('checked', false);
    $('#etf-tab .pars input').attr('checked', false);
    var provider = $('#etf_data_provider_select').val();
    if (provider == 0) $('#etf-tab .cp').show();
    else
    {
      $('#etf-tab .pars').hide();
      $('#etf_leveraged_div .pars').show();
      
      $.post('get_filters.php',
        {
          type: 4,
          provider: provider,
          what: 'category'
        },
        function(data){
          for (var i in data)
          {
            $('#etf_cat_div .pars[cat_id="'+data[i]+'"]').show();
            $('#etf_cat_div .pars[cat_id="'+data[i]+'"] input').attr('checked', true);
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 4,
          provider: provider,
          what: 'region'
        },
        function(data){
          for (var i in data)
          {
            $('#etf_region_div .pars[region_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 4,
          provider: provider,
          what: 'exchange'
        },
        function(data){
          for (var i in data)
          {
            $('#etf_data_market_div .pars[exchange_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 4,
          provider: provider,
          what: 'issuer'
        },
        function(data){
          for (var i in data)
          {
            $('#etf_issuer_div .pars[issuer_id="'+data[i]+'"]').show();
            $('#etf_issuer_div .pars[issuer_id="'+data[i]+'"] input').attr('checked', true);
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 4,
          provider: provider,
          what: 'assetclass'
        },
        function(data){
          for (var i in data)
          {
            $('#etf_assetclass_div .pars[asset_class_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 4,
          provider: provider,
          what: 'assetsize'
        },
        function(data){
          for (var i in data)
          {
            $('#etf_assetsize_div .pars[asset_class_size_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
      
      $.post('get_filters.php',
        {
          type: 4,
          provider: provider,
          what: 'assetstyle'
        },
        function(data){
          
          for (var i in data)
          {
            $('#etf_assetstyle_div .pars[asset_class_style_id="'+data[i]+'"]').show();
          }
        },
        'json'
      );
      
      
      var intervalID = setInterval(function() {
        if (
          $('#etf-tab .pars[exchange_id] input:visible').length > 0
          && $('#etf-tab .pars[asset_class_id] input:visible').length > 0
          && $('#etf-tab .pars[asset_class_style_id] input:visible').length > 0
          && $('#etf-tab .pars[asset_class_size_id] input:visible').length > 0
        )
        {
          clearInterval(intervalID);
          $('#etf-tab .pars input:visible').attr('checked', true);
          show_etf();
        }
      }, 100);
    }
  }
  
  
  $('#etf_data_provider_select').change(etf_data_provider_select_change);
  etf_ajax = 0;
  function get_etf()
  {
    if (etf_ajax == 1) return true;
    $('#etf_loading').show();
    var provider = $('#etf_data_provider_select').val();
    var exchanges = [];
    $('#etf-tab .pars[exchange_id] input:checked:visible').each(function(){
      exchanges.push($(this).val());
    });
    var issuers = [];
    $('#etf-tab .pars[issuer_id] input:checked').each(function(){
      issuers.push($(this).val());
    });
    var cats = [];
    $('#etf-tab .pars[cat_id] input:checked').each(function(){
      cats.push($(this).val());
    });
    
    var assets = [];
    $('#etf-tab .pars[asset_class_id] input:checked:visible').each(function(){
      assets.push($(this).val());
    });
    var assetsizes = [];
    $('#etf-tab .pars[asset_class_size_id] input:checked:visible').each(function(){
      assetsizes.push($(this).val());
    });
    var assetstyles = [];
    $('#etf-tab .pars[asset_class_style_id] input:checked:visible').each(function(){
      assetstyles.push($(this).val());
    });

    var undindex = $('#etf_und_index').val();
    
    if (arguments[0]!=1) etf_limit = 0;
    
    etf_ajax = 1;
    $.post('get_etf.php',
      {
        'exchanges[]': exchanges,
        'issuers[]': issuers,
        'cats[]': cats,
        'assets[]': assets,
        'assetsizes[]': assetsizes,
        'assetstyles[]': assetstyles,
        undindex: undindex,
        leveraged_on: $('#etf_leveraged_on').is(':checked'),
        leveraged_off: $('#etf_leveraged_off').is(':checked'),
        provider: provider,
        limit: etf_limit
      },
      function(data){
        
        $('#etf_loading').hide();
        for (var i in data.res)
        {
          $('#etf_table tbody').append('<tr class="cp" exchange_id="'+data.res[i].exchange_id+'" style="display:table-row">\
<td><input type=checkbox name=etf[] value="'+data.res[i].id+'-'+data.res[i].exchange_id+'"></td>\
<td>'+data.res[i].symbol+'</td>\
<td nowrap>'+data.res[i].exchange+'</td>\
<td nowrap>'+data.res[i].name+'</td>\
<td>'+data.res[i].undindex_name+'</td>\
<td>'+data.res[i].sector+'</td>\
<td>'+data.res[i].issuer_name+'</td>\
<td>'+data.res[i].assetclass_name+'</td>\
<td>'+data.res[i].assetsize_name+'</td>\
<td>'+data.res[i].assetstyle_name+'</td>\
</tr>');

        }
        $('#etf_found span').html(data.count);
        etf_limit += 100;
        etf_ajax = 0;
        $('#etf_table').show();
        //$('#etf_table').tablesorter({ headers: {0: { sorter: false}, 2: { sorter: false}, 3: { sorter: false}, 5: { sorter: false}, 6: { sorter: false}, 7: { sorter: false}, 8: { sorter: false}, 9: { sorter: false}} });
        
      },
      'json'
    );
  }
  
  function show_etf()
  {
    $('#etf_table tbody').empty();
    $('#etf_table').hide();
    get_etf();
    
    docalc();
    return true;

  }
  
  function on_region_changed3(t)
  {
    //$('#futures_data_market_div input').attr('checked', false);
    $('#etf_data_market_div .pars[region_id='+$(this).val()+'] input').attr('checked', $(this).is(':checked'));
  }
  
  function on_region_changed_all3()
  {
    $('#etf_data_market_div input').attr('checked', false);
    $('#etf_region_div .pars input:checked').each(function(){
      $('#etf_data_market_div .pars[region_id='+$(this).val()+'] input').attr('checked', true);
    });
  }
  

</script>

<div class='form-title1' style="clear: both;">Select Date Range: <span id="date_range_span" style="font-weight:normal"></span></DIV>
<style>
#daterange_slider a {
  background: url("images/ui-bg_glass_75_dadada_1x400.png") repeat-x scroll 50% 50% #DADADA;
  border: 1px solid #999999;
  color: #212121;
}
</style>
<div id="daterange_slider" style="margin-top:5px;width:300px;float:left;"></div>
<div style="margin-left:-5px;margin-top:2px;float:left;"><img src="/img/slider.gif" alt="" border=0></div>
<input type="hidden" name="daterange_from" id="daterange_from">
<input type="hidden" name="daterange_to" id="daterange_to">
<div style="margin-left:30px;float:left;">
<?php $todate = strtotime('+1 day', strtotime($DCS_OPTIONS['data_update_date'])); ?>
<?php $d = strtotime('-3 months', $todate)?><a href="#" onclick="setdatefrom(this,<?=date('Y', $d)?>,<?=date('n', $d)?>,<?=date('j', $d)?>);return false;" class="dateperiod">3 mo</a>
<?php $d = strtotime('-6 months', $todate)?>&nbsp; <a href="#" onclick="setdatefrom(this,<?=date('Y', $d)?>,<?=date('n', $d)?>,<?=date('j', $d)?>);return false;" class="dateperiod">6 mo</a>
<?php $d = strtotime('-1 year', $todate)?>&nbsp; <a href="#" onclick="setdatefrom(this,<?=date('Y', $d)?>,<?=date('n', $d)?>,<?=date('j', $d)?>);return false;" class="dateperiod">1 yr</a>
<?php $d = strtotime('-2 years', $todate)?>&nbsp; <a href="#" onclick="setdatefrom(this,<?=date('Y', $d)?>,<?=date('n', $d)?>,<?=date('j', $d)?>);return false;" class="dateperiod">2 yr</a>
<?php $d = strtotime('-10 years', $todate)?>&nbsp; <a href="#" onclick="setdatefrom(this,<?=date('Y', $d)?>,<?=date('n', $d)?>,<?=date('j', $d)?>);return false;" class="dateperiod" id="maxdateperiod">max</a>


<script>
function setdatefrom(t,y,m,d)
{
  
  $('.dateperiod').removeClass('dateperiodselected');
  $(t).addClass('dateperiodselected');
  //var d = $('#datepickerto').datepicker('getDate');
  //var d2 = new Date();
  //d2.setTime(d.getTime()-p*1000);
  var newt = 12*y+m;
  if (newt < $('#daterange_slider').slider( 'option', 'min')) newt = $('#daterange_slider').slider( 'option', 'min' );
  $('#daterange_slider').slider( 'value', newt );
  //$('#datepickerfrom').datepicker('setDate', new Date(y,m-1,d));
  
  /*
    var d2 = $('#datepickerto').datepicker('getDate');
    var str = '';
    if (p=='1m') str = ' -3m';
    $('#datepickerfrom').datepicker('setDate', d2 + str);
    */
}

</script>
</div>
<br clear=all>
         
<div class='form-title1'>Select Data Update Frequency</div>
<SELECT name='dataload' id='dataload' onchange="docalc();" class='form-input1 form-inputw3'>
<OPTION value=50>Monthly</OPTION>
</SELECT>
<div class='form-title1'>Enter Subscription Length (# of months)</div>
<SELECT name='month' id='monthno' onchange="docalc();" class='form-input1 form-inputw3'>
<option value="1"<?php if (intval($subinfo['months'])==1) echo ' selected'?>>1
<option value="2"<?php if (intval($subinfo['months'])==2) echo ' selected'?>>2
<option value="3"<?php if (intval($subinfo['months'])==3) echo ' selected'?>>3
<option value="4"<?php if (intval($subinfo['months'])==4) echo ' selected'?>>4
<option value="5"<?php if (intval($subinfo['months'])==5) echo ' selected'?>>5
<option value="6"<?php if (intval($subinfo['months'])==6) echo ' selected'?>>6
<option value="7"<?php if (intval($subinfo['months'])==7) echo ' selected'?>>7
<option value="8"<?php if (intval($subinfo['months'])==8) echo ' selected'?>>8
<option value="9"<?php if (intval($subinfo['months'])==9) echo ' selected'?>>9
<option value="10"<?php if (intval($subinfo['months'])==10) echo ' selected'?>>10
<option value="11"<?php if (intval($subinfo['months'])==11) echo ' selected'?>>11
<option value="12"<?php if (intval($subinfo['months'])==12) echo ' selected'?>>12
</SELECT>

<!--
<div class='form-title1'>DCS Availability Time Slot</div>
<SELECT name='availability' id='availability' onchange="docalc();" class='form-input1 form-inputw3'>
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
<div style="font-size: 8pt">DCS is made available in 12 hour time slots per day. Below, select the hour of the day, relative to UTC (Coordinated Universal Time), that you want your 12 hour time slot to start. <u>DCS will automatically be unavailable to you 12 hours later.</u> Alternatively, you may select "24 hour access", which will increase the monthly fee.
</div>
-->
<div class='spacer'></div>
<!--
<DIV class='feee'> = $<SPAN id='fee1'>0.00</SPAN></DIV>Base Monthly Fee<br>
<DIV class='feee'> = $<SPAN id='fee_availability'>0.00</SPAN></DIV>Extended Access Monthly Fee<br>
<DIV class='feee'> = $<SPAN id='fee2'>0.00</SPAN></DIV>Update Monthly Fee<br>
<DIV class='feee'> = $<SPAN id='fee3'>0.00</SPAN></DIV>Gross Monthly Fee<br>
-->
<DIV class='feee'> = $<SPAN id='fee4'>0.00 ( 0% )</SPAN></DIV><DIV class='feeelabel'>Discounts start with a subscription of three months or more</DIV>
Less: Discount <input type=text id="promocode" name="promocode" style="width:50px">
<div id="discloading" style="display:none"><img src="/loading4.gif" alt="" border=0></div>
<br>
<script>$('#promocode').blur(check_discount);</script>
<DIV class='feee'> = $<SPAN id='fee5'>0.00</SPAN></DIV>Monthly Fees<br>
<DIV class='feee'> = <STRONG>$<SPAN id='fee7'>0.00</SPAN></STRONG></DIV>Total Payment for <SPAN id='fee6'>12 Months</SPAN><br>
<div class='spacer'></div><input type='submit' value='Next Step' disabled id='nextbutton' class='form-input2'>
</form></div>

<?
         } else if ($REGISTER['step'] == 2){
             
                echo "<div id='reg'><form method=post action='subscribe/'><input type=radio name='isnewuser' value=0 checked> I have account on DCS<br>";
                echo "<input type=radio name='isnewuser' value=1> New user<br>";
                echo "<br><input type='submit' value='Next Step' id='nextbutton' class='form-input2'></form></div>";
                
                } else if ($REGISTER['step'] == 3){
?>

<DIV class='notice'><?=$LANG['TRIAL']['TEXT1'];?></DIV>
<div id='reg'>
<FORM name='formr' method=post action='subscribe/'>

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

<div class='form-title1'>Address</div>
<INPUT type='text' name='city' value='<?=$REGISTER['city'];?>' class='form-input1 form-inputw2'>
<INPUT type='text' name='state_province_region' value='<?=$REGISTER['state_province_region'];?>' class='form-input1 form-inputw2'>
<INPUT type='text' name='zipcode' value='<?=$REGISTER['zipcode'];?>' class='form-input1 form-inputw2'>
<div class='form-title4'>Zip / Postal Code</div>
<div class='form-title3'>State / Province / Region</div>
<div class='form-title2'>
<b>City</b> <SPAN class='mandatory'>*</SPAN>
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
<INPUT type='text' name='linkedin' value='<? echo (!empty($REGISTER['linkedin'])?$REGISTER['linkedin']:'');?>' class='form-input1 form-inputw2'>
<div class='form-title4'>LinkedIn Profile
<? if (isset($E['linkedin'])) echo "<DIV class='mandatory'>$E[linkedin]</DIV>";?>
</div>
<div class='form-title3'><b>Phone Number</b> <SPAN class='mandatory'>*</SPAN>
<? if (isset($E['phonenumber'])) echo "<DIV class='mandatory'>$E[phonenumber]</DIV>";?>
</div>
<div class='form-title2'><b>Country</b> <SPAN class='mandatory'>*</SPAN></div>

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
<br>

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

<INPUT TYPE=submit class='form-input2' value='Next Step'>
</FORM>
</div>
<?                    
                                            
                     } else if ($REGISTER['step'] == 4)
                     {
                                echo "<div id='reg'><form method=post action='subscribe/'>";
                                echo "<input type=hidden name='dologin'>";
                                echo "<div class='form-title5'>Password <SPAN class='mandatory'>*</SPAN></div>";
                                echo "<div class='form-title1'>Username <SPAN class='mandatory'>*</SPAN></div>";
                                if (isset($E['username'])) echo "<DIV class='mandatory'>$E[username]</DIV>";
                                if (isset($E['password'])) echo "<DIV class='mandatory'>$E[password]</DIV>";
                                echo "<input type=text name='username' class='form-input1 form-inputw2'>";
                                echo "<input type=password name='password' class='form-input1 form-inputw2'>";                                
                                echo "<br><br><input type='submit' id='nextbutton' value='Next Step' class='form-input2'></form></div>";
                     }
                     else if ($REGISTER['step'] == 5)
                     {
                        if ($REGISTER['mode'] == 'renew')
                        {
                          $next = mysql_fetch_assoc(mysql_query('select * from subscription where id="'.$REGISTER['billid'].'"', $lnk));
                          if (empty($next['can_renew'])) exit('Error: Can\'t renew this subscription');
                          if (empty($next['id'])) exit('Error: Can\'t find your current subscription');
                          //strtotime($next['finishdate']) - strtotime($next['dos'])
                          $sql = "
                            INSERT INTO subscription (
                              userid, dos, finishdate,
                              dr_from, dr_to, options,
                              dataupdate, months, availability,
                              fee1, fee2, fee3,
                              fee4, fee5, fee6,
                              us_calendar, can_renew
                            )
                            VALUES(
                              ".$next['userid'].", '".$next['finishdate']."', '".date('Y-m-d H:i:s', strtotime('+'.intval($next['months']).' months', strtotime($next['finishdate'])))."',
                              '".$next['dr_from']."', '".$next['dr_to']."', '".$next['options']."',
                              ".$next['dataupdate'].", ".$next['months'].", '".$next['availability']."',
                              '".$next['fee1']."', '".$next['fee2']."', '".$next['fee3']."',
                              '".$next['fee4']."', '".$next['fee5']."', '".$next['fee6']."',
                              '".$next['us_calendar']."', 0
                            )
                          ";
                          @mysql_query($sql, $lnk) or die(mysql_error($lnk));
                          $REGISTER['billid'] = mysql_insert_id($lnk);
                          $REGISTER['TOTALFEE'] = $next['fee6'];
                          $REGISTER['userid'] = $next['userid'];
                        }
                        //echo "Proceed to Paypal with USERID = $REGISTER[userid] , BILLID = $REGISTER[billid] and TOTALFEE = $REGISTER[TOTALFEE]<br>";
                        $next = mysql_fetch_assoc(mysql_query('select * from subscription where id='.intval($REGISTER['billid']), $lnk));
                        
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
                          $next2 = mysql_fetch_assoc(mysql_query('select * from data_products where id="'.intval($v).'"', $lnk));
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
                          $next2 = mysql_fetch_assoc(mysql_query('select * from data_barsizes where id="'.intval($v).'"', $lnk));
                          if (!empty($next2['bar_size_text'])) $barsize[] = $next2['bar_size_text'];
                        }
                        $barsize = implode(', ', $barsize);

                        preg_match('`futures\:(.+)\n`U', $next['options'], $m);
                        $tmp = explode(',', $m[1]);
                        $futures = array();
                        foreach ($tmp as $v)
                        {
                          $v = trim($v); if (empty($v)) continue;
                          $next2 = mysql_fetch_assoc(mysql_query('select * from data_products where id="'.intval($v).'"', $lnk));
                          if (!empty($next2['symbol'])) $futures[] = $next2['symbol'];
                        }
                        $futures = implode(', ', $futures);
                        preg_match('`futures_barsize\:(.+)\n`U', $next['options'], $m);
                        $tmp = explode(',', $m[1]);
                        $futures_barsize = array();
                        foreach ($tmp as $v)
                        {
                          $v = trim($v); if (empty($v)) continue;
                          $next2 = mysql_fetch_assoc(mysql_query('select * from data_barsizes where id="'.intval($v).'"', $lnk));
                          if (!empty($next2['bar_size_text'])) $futures_barsize[] = $next2['bar_size_text'];
                        }
                        $futures_barsize = implode(', ', $futures_barsize);
                        
                        echo '<b>Subscription details:</b><br>';
                        if (!empty($ccy))
                        {
                          echo 'Currency pairs: '.$ccy.'<br>';
                          echo 'Bar Size(s): '.$barsize.'<br>';
                        }
                        if (!empty($futures))
                        {
                          echo 'Future Products: '.$futures.'<br>';
                          echo 'Contract Types: Continuous<br>';
                          echo 'Bar Size(s): '.$futures_barsize.'<br>';
                        }
                        ?>
                        Other Data Sources: <?php echo $next['us_calendar'] ? 'World Economic Calendar from Econoday.com' : 'No'?><br>
                        Date Range: from <?php echo date('j F Y', strtotime($next['dr_from']))?> to <?php echo date('j F Y', strtotime($next['dr_to']))?><br>
                        Data Update Frequency: <?php echo $dataupdate?><br>
                        Subscription Length: <?php echo intval($next['months'])?> month(s)
                        <div style="height:10px"></div>
                        <b>Amount To Pay: $<?php echo sprintf("%01.2f", $next['fee6'])?></b><br>
                        <br>
                        <?
                        
                        if ($REGISTER['TOTALFEE'] > 0) echo '<form method="post" action="https://www.'.($REGISTER['bycode']==2?'sandbox.':'').'paypal.com/cgi-bin/webscr">';
                        else echo '<form method="post" action="https://dcs.deltixlab.com/paypalipn.php">';
?>
<input type="hidden" name="charset" value="utf-8">
<input type="hidden" name="cmd" value="_xclick">
<input type="hidden" name="amount" value="<?=$REGISTER['TOTALFEE'];?>">
<input type="hidden" name="currency_code" value="USD">
<input type="hidden" name="item_name" value="DCS Personal Subscription Plan">
<input type="hidden" name="item_number" value="<?=$REGISTER['billid']?>">
<input type="hidden" name="business" value="<? echo ($REGISTER['bycode']==2?'korvin_1338207199_biz@gmail.com':'ilya@deltixlab.com')?>">
<input type="hidden" name="custom" value="<?=$REGISTER['userid'];?>">
<input type="hidden" name="lc" value="US">
<input type="hidden" name="button_subtype" value="services">
<input type="hidden" name="no_note" value="1">
<input type="hidden" name="no_shipping" value="1">
<input type="hidden" name="rm" value="1">
<input type="hidden" name="quantity" value="1">
<input type="hidden" name="undefined_quantity" value="0">
<input type="hidden" name="return" value="https://dcs.deltixlab.com/subscribe/success/">
<input type="hidden" name="cancel_return" value="https://dcs.deltixlab.com/subscribe/cancel/">
<input type="hidden" name="notify_url" value="https://dcs.deltixlab.com/paypalipn.php">
<input type="hidden" name="bn" value="PP-BuyNowBF:btn_buynow_SM.gif:NonHostedGuest">
<?php
if ($REGISTER['TOTALFEE'] == 0) echo '<input type="submit" name="submit" value="Please click to complete your subscription">';
else echo '<input type="image" src="https://www.'.($REGISTER['bycode']==2?'sandbox.':'').'paypal.com/en_US/i/btn/btn_buynow_SM.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">';
?>
<img alt="" border="0" src="https://www.<? echo ($REGISTER['bycode']==2?'sandbox.':'')?>paypal.com/en_US/i/scr/pixel.gif" width="1" height="1">
</form><br>
<?
                     }
         }
//===================================================================================================================================

?>
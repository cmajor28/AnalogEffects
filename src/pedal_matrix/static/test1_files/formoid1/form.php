<?php

define('EMAIL_FOR_REPORTS', '');
define('RECAPTCHA_PRIVATE_KEY', '@privatekey@');
define('FINISH_URI', 'http://');
define('FINISH_ACTION', 'message');
define('FINISH_MESSAGE', 'Thanks for filling out my form!');
define('UPLOAD_ALLOWED_FILE_TYPES', 'doc, docx, xls, csv, txt, rtf, html, zip, jpg, jpeg, png, gif');

define('_DIR_', str_replace('\\', '/', dirname(__FILE__)) . '/');
require_once _DIR_ . '/handler.php';

?>

<?php if (frmd_message()): ?>
<link rel="stylesheet" href="<?php echo dirname($form_path); ?>/formoid-biz-red.css" type="text/css" />
<span class="alert alert-success"><?php echo FINISH_MESSAGE; ?></span>
<?php else: ?>
<!-- Start Formoid form-->
<link rel="stylesheet" href="<?php echo dirname($form_path); ?>/formoid-biz-red.css" type="text/css" />
<script type="text/javascript" src="<?php echo dirname($form_path); ?>/jquery.min.js"></script>
<form class="formoid-biz-red" style="background-color:#1A2223;font-size:14px;font-family:'Open Sans','Helvetica Neue', 'Helvetica', Arial, Verdana, sans-serif;color:#ECECEC;max-width:768px;min-width:150px" method="post"><div class="title"><h2>Pedal Matrix</h2></div>
	<div class="element-separator"><hr><h3 class="section-break-title">Bank Info</h3></div>
	<div class="element-input<?php frmd_add_class("input"); ?>"><label class="title"></label><input class="large" type="text" name="input" placeholder="Bank Name"/></div>
	<div class="element-select<?php frmd_add_class("select7"); ?>"><label class="title"></label><div class="medium"><span><select name="select7" >

		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
		<option value="5">5</option>
		<option value="6">6</option>
		<option value="7">7</option>
		<option value="8">8</option>
		<option value="9">9</option>
		<option value="10">10</option>
		<option value="11">11</option>
		<option value="12">12</option>
		<option value="13">13</option>
		<option value="14">14</option>
		<option value="15">15</option>
		<option value="16">16</option></select><i></i></span></div></div>
	<div class="element-separator"><hr><h3 class="section-break-title">Preset Info</h3></div>
	<div class="element-input<?php frmd_add_class("input1"); ?>"><label class="title"></label><input class="large" type="text" name="input1" placeholder="Preset Name"/></div>
	<div class="element-select<?php frmd_add_class("select8"); ?>"><label class="title"></label><div class="medium"><span><select name="select8" >

		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
		<option value="5">5</option>
		<option value="6">6</option>
		<option value="7">7</option>
		<option value="8">8</option></select><i></i></span></div></div>
	<div class="element-separator"><hr><h3 class="section-break-title">Pedal Order</h3></div>
	<div class="element-select<?php frmd_add_class("select"); ?>"><label class="title"></label><div class="medium"><span><select name="select" >

		<option value="0">0</option>
		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
		<option value="5">5</option>
		<option value="6">6</option>
		<option value="7">7</option></select><i></i></span></div></div>
	<div class="element-checkbox<?php frmd_add_class("checkbox"); ?>"><label class="title">Pedal 1 Enabled?</label>		<div class="column column1"><label><input type="checkbox" name="checkbox[]" value="Yes"/ ><span>Yes</span></label></div><span class="clearfix"></span>
</div>
	<div class="element-select<?php frmd_add_class("select1"); ?>"><label class="title"></label><div class="medium"><span><select name="select1" >

		<option value="0">0</option>
		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
		<option value="5">5</option>
		<option value="6">6</option>
		<option value="7">7</option></select><i></i></span></div></div>
	<div class="element-checkbox<?php frmd_add_class("checkbox3"); ?>"><label class="title">Pedal 2 Enabled?</label>		<div class="column column1"><label><input type="checkbox" name="checkbox3[]" value="Yes"/ ><span>Yes</span></label></div><span class="clearfix"></span>
</div>
	<div class="element-select<?php frmd_add_class("select4"); ?>"><label class="title"></label><div class="medium"><span><select name="select4" >

		<option value="0">0</option>
		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
		<option value="5">5</option>
		<option value="6">6</option>
		<option value="7">7</option></select><i></i></span></div></div>
	<div class="element-checkbox<?php frmd_add_class("checkbox4"); ?>"><label class="title">Pedal 3 Enabled?</label>		<div class="column column1"><label><input type="checkbox" name="checkbox4[]" value="Yes"/ ><span>Yes</span></label></div><span class="clearfix"></span>
</div>
	<div class="element-select<?php frmd_add_class("select6"); ?>"><label class="title"></label><div class="medium"><span><select name="select6" >

		<option value="0">0</option>
		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
		<option value="5">5</option>
		<option value="6">6</option>
		<option value="7">7</option></select><i></i></span></div></div>
	<div class="element-checkbox<?php frmd_add_class("checkbox6"); ?>"><label class="title">Pedal 4 Enabled?</label>		<div class="column column1"><label><input type="checkbox" name="checkbox6[]" value="Yes"/ ><span>Yes</span></label></div><span class="clearfix"></span>
</div>
	<div class="element-select<?php frmd_add_class("select5"); ?>"><label class="title"></label><div class="medium"><span><select name="select5" >

		<option value="0">0</option>
		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
		<option value="5">5</option>
		<option value="6">6</option>
		<option value="7">7</option></select><i></i></span></div></div>
	<div class="element-checkbox<?php frmd_add_class("checkbox5"); ?>"><label class="title">Pedal 5 Enabled?</label>		<div class="column column1"><label><input type="checkbox" name="checkbox5[]" value="Yes"/ ><span>Yes</span></label></div><span class="clearfix"></span>
</div>
	<div class="element-select<?php frmd_add_class("select3"); ?>"><label class="title"></label><div class="medium"><span><select name="select3" >

		<option value="0">0</option>
		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
		<option value="5">5</option>
		<option value="6">6</option>
		<option value="7">7</option></select><i></i></span></div></div>
	<div class="element-checkbox<?php frmd_add_class("checkbox2"); ?>"><label class="title">Pedal 6 Enabled?</label>		<div class="column column1"><label><input type="checkbox" name="checkbox2[]" value="Yes"/ ><span>Yes</span></label></div><span class="clearfix"></span>
</div>
	<div class="element-select<?php frmd_add_class("select2"); ?>"><label class="title"></label><div class="medium"><span><select name="select2" >

		<option value="0">0</option>
		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
		<option value="5">5</option>
		<option value="6">6</option>
		<option value="7">7</option></select><i></i></span></div></div>
	<div class="element-checkbox<?php frmd_add_class("checkbox1"); ?>"><label class="title">Pedal 7 Enabled?</label>		<div class="column column1"><label><input type="checkbox" name="checkbox1[]" value="Yes"/ ><span>Yes</span></label></div><span class="clearfix"></span>
</div>
	<div class="element-separator" title="If the TRS Jack is enabled to the output"><hr><h3 class="section-break-title">External Output Enabled</h3></div>
	<div class="element-checkbox<?php frmd_add_class("checkbox8"); ?>"><label class="title">TRS Enable 1:</label>		<div class="column column1"><label><input type="checkbox" name="checkbox8[]" value="Yes"/ ><span>Yes</span></label></div><span class="clearfix"></span>
</div>
	<div class="element-checkbox<?php frmd_add_class("checkbox7"); ?>"><label class="title">TRS Enable 2:</label>		<div class="column column1"><label><input type="checkbox" name="checkbox7[]" value="Yes"/ ><span>Yes</span></label></div><span class="clearfix"></span>
</div>
<div class="submit"><input type="submit" value="Submit"/></div></form><script type="text/javascript" src="<?php echo dirname($form_path); ?>/formoid-biz-red.js"></script>

<!-- Stop Formoid form-->
<?php endif; ?>

<?php frmd_end_form(); ?>
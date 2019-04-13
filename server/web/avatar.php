<?php
require_once("imports.php");
$wi = new WebInterface();
$avatar = $wi->dbConnection->GetAvatar(intval($_GET["uid"]));
$content_type = "image/bmp";
$filename = "avatar.bmp";

if (is_null($avatar))
{
	$avatar = file_get_contents("noavatar.bmp");
}
header('Content-type: '.$content_type);
header('Content-Disposition: inline; filename="'.$filename.'"');
print($avatar);

?>
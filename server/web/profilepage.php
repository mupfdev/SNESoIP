<?php
/* profilepage.php -*-web-*-
 * User profile settings page 
 * Author: Daniel Baumann
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


require_once "imports.php";
session_start();

if (!isset($_SESSION["currentuser"]))
{
	header("Location: index.php");
}

$wi = new WebInterface();

if (!$wi->isConnected)
{
	die();
}

if ($_SERVER['REQUEST_METHOD'] == 'POST')
{
	if (isset($_POST["hwid"]))
	{
		$wi->dbConnection->SetOpponent($_POST["hwid"], $_POST["opponentid"]);
	}
	else
	{
		if (isset ($_POST["password"]) && !empty($_POST["password"]))
		{
			$passworderror = false;
			if ($_POST["password"] != $_POST["repassword"])
			{
				$passworderror = true;
			}
			else
			{
				$newPW = password_hash($_POST["password"], PASSWORD_DEFAULT);
				if (!$wi->dbConnection->SetPassword($_SESSION["currentuser"], $newPW))
				{
					$passworderror = true;
				}
			}
		}

		if (isset($_FILES) && isset($_FILES["avatar"]))
		{
			if (!is_null($_FILES["avatar"]["tmp_name"]))
			{
				if ($_FILES["avatar"]["type"] != "image/bmp")
				{
					$wrongFileType = true;
				}
				else
				{
					$handle = fopen($_FILES["avatar"]["tmp_name"], "rb");
					$bmp = fread($handle, filesize($_FILES["avatar"]["tmp_name"]));
					if (is_null($bmp))
					{
						$uploadError = true;
					}
					else					
					{
						$width = unpack('i', substr($bmp,18,4));
						$height = unpack('i', substr($bmp,22,4));
						$bpp = unpack('s',substr($bmp,28,2));
						// Check if the bmp file has only 16 colors and is not larger than 32px x 32px
						if ($width[1] > 32 || $height[1] > 32 || $bpp[1] > 4)
						{
							$fileformaterror = true;
						}
						else
						{
							$wi->dbConnection->SetAvatar($_SESSION["currentuser"],$bmp);
						}
					}
				}
			}			
			
		}

		$updatedProfile = new Profile();
		$updatedProfile->email = $_POST["email"];
		$updatedProfile->realName = $_POST["realname"];
		$updatedProfile->region = $_POST["region"];

		$wi->dbConnection->UpdateProfile($updatedProfile, $_SESSION["currentuser"]);
	}
}

$user = $wi->dbConnection->GetUserByID($_SESSION["currentuser"]);

?>
<!doctype html>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1.0" />
		<title>SNESoIP server</title>
		<link rel="stylesheet" href="style.css" type="text/css" />

		<!--[if lt IE 9]>
		<script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"></script>
		<![endif]-->
	</head>
	<body>

		<div class="box">
			<h1><a href=".">SNESoIP server</a></h1>
			<div>
				<h2>Logged in as <?php print htmlspecialchars($user->name); ?> (0)</h2>
				<p class="logout"><a href="index.php?a=logout">Logout</a>
				<?php if ($user->is_admin) {?>
					<a href="admin.php">Admin</a>
				<?php  } ?>
				</p>
				<div class="logo"><img src="gfx/snes-logo.svg" alt="SNES" /></div>
			</div>

			<div>
				<?php if (isset($passworderror) && $passworderror) {?>
					<p>Could not update password!</p>
				<?php }?>
				
				<?php if (isset($wrongFileType) && $wrongFileType) {?>
					<p>Avatar has to be a BMP file!</p>
				<?php }?>				

				<?php if (isset($fileformaterror) && $fileformaterror) {?>
					<p>Your avatar must not be larger than 32px in width and height. The maximum color depth is 4 bits per pixel!</p>
				<?php }?>

				<?php if (isset($uploadError) && $uploadError) {?>
					<p>Could not upload your avatar. Please contact the server administrator for help.</p>
				<?php }?>
							
				<form id="profile" action="profilepage.php" method="post" enctype="multipart/form-data">
					<p class="realname">
						<label for="realname">Real name</label>
						<input id="realname" type="text" name="realname" value="<?php echo htmlspecialchars($user->profile->realName); ?>" autocomplete="off">
					</p>
					<p class="email">
						<label for="email">E-Mail</label>
						<input id="email" type="text" name="email" value="<?php echo htmlspecialchars($user->profile->email); ?>" autocomplete="off">
					</p>
					<p class="region">
						<label for="region">Region</label>
						<input id="region" type="text" name="region" value="<?php echo htmlspecialchars($user->profile->region); ?>" autocomplete="off">
					</p>
					<p class="password">
						<small>Leave empty to keep old password</small>
						<label for="password">Password</label>
						<input id="password" type="password" name="password" value="" autocomplete="off">
					</p>
					<p class="repassword">
						<label for="repassword">Validate Password</label>
						<input id="repassword" type="password" name="repassword" value="" autocomplete="off">
					</p>
					<p class="avatarupload">
						<label for="avatar">Avatar</label>
						<input id="avatar" type="file" name="avatar"/>
					</p>
					<p class="avatar">
						<img src="avatar.php?uid=<?php echo $_SESSION['currentuser'];?>" alt="avatar" class="avatar"/>
					</p>
					<p>
						<button name="save">»&nbsp;&nbsp;Save</button>
					</p>
				</form>
					<?php
						$devices = $wi->dbConnection->GetUserDevices($user->id);
						foreach ($devices as $device)
						{
					?>
						<form id="setoponnent_<?php print $device->hwID; ?>" action="profilepage.php" method="post">
							<input type="hidden" name="hwid" value="<?php print $device->hwID; ?>"/>
							<p class="hardware">
								<h3>Your SNESoIPs ID: <?php print $device->hwID; ?></h3>
								<label for="remid_<?php echo $device->hwID; ?>">Remote ID</label>
								<select id="remid_<?php echo $device->hwID; ?>" name="opponentid" required>
								<?php
									$otherDevices = $wi->dbConnection->GetListOfAllDevices(false);
									print HtmlHelper::GetDeviceComboBoxOptions($otherDevices, $device->opponentID);
								?>
								</select>

							</p>
							<p class="ip">
								<label for="ip_<?php echo $device->hwID; ?>">IP address</label>
								<input id="ip_<?php echo $device->hwID; ?>" type="text" name="ip" value="<?php echo htmlspecialchars($device->currentIP);?>" readonly="readonly">
							</p>
							<p>
								<button name="setoponnent">Set opponent</button>
							</p>
						</form>
					<?php } ?>

					<p>
						Your current IP address:&nbsp;
						<?php
						if (!empty($_SERVER["HTTP_CLIENT_IP"])) {
							$ip = $_SERVER["HTTP_CLIENT_IP"];
						}
						elseif (!empty($_SERVER["HTTP_X_FORWARDED_FOR"])) {
							$ip = $_SERVER["HTTP_X_FORWARDED_FOR"];
						}
						else {
							$ip = $_SERVER["REMOTE_ADDR"];
						}
						echo $ip;
						?>
					</p>
			</div>
		</div>

		<footer>
			<p>Visit the SNESoIP project repository on <a href="https://github.com/mupfelofen-de/SNESoIP">GitHub</a>.</p>
		</footer>

	</body>
</html>

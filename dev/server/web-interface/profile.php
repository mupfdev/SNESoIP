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
				<h2>Logged in as johnd (0)</h2>
				<p class="logout"><a href=".">Logout</a></p>
				<div class="logo"><img src="gfx/snes-logo.svg" alt="SNES" /></div>
			</div>

			<div>
				<form id="profile" action="profile.php" method="post">
					<p class="realname">
						<label for="realname">Real name</label>
						<input id="realname" type="text" name="realname" value="John Doe" autocomplete="off">
					</p>
					<p class="email">
						<label for="email">E-Mail</label>
						<input id="email" type="text" name="email" value="john@doe.com" autocomplete="off">
					</p>
					<p class="remid">
						<label for="remid">Remote ID</label>
						<input id="remid" type="number" name="remid" min="0" max="255" value="1" required>
					</p>
					<p class="ip">
						<label for="ip1">IP address</label>
						<input id="ip1" type="number" name="ip1" min="0" max="255" value="109" required>.
						<input type="number" name="ip2" min="0" max="255" value="193" required>.
						<input type="number" name="ip3" min="0" max="255" value="192" required>.
						<input type="number" name="ip4" min="0" max="255" value="24" required>
					</p>
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
					<p>
						<button name="save">»&nbsp;&nbsp;Save</button>
					</p>
				</form>
			</div>
		</div>

		<footer>
			<p>Visit the SNESoIP project repository on <a href="https://github.com/mupfelofen-de/SNESoIP">GitHub</a>.</p>
		</footer>

	</body>
</html>

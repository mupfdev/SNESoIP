<?php
class HtmlHelper
{
	public static function GetDeviceComboBoxOptions($devices, $selectedID = 0)
	{
		$html = "";
		$html .= '<option value="-1">None</option>';
		foreach ($devices as $device)
		{
			$html .= '<option value="'.$device["hw"]->hwID.'"';
			if (($device["hw"]->hwID == $selectedID))
			{
				$html .= "selected";	
			}
			$html.= '>'.htmlspecialchars($device["owner"]).' ('.  $device["hw"]->hwID. ')</option>';
			
		}
		return $html;
	}
	
	public static function GetUserComboBoxOptions($users)
	{
		$html = "";
		foreach ($users as $user)
		{
			$html .= '<option value="'.$user->id.'">'.htmlspecialchars($user->name). '</option>';
		}
		return $html;		
	}
}
?>
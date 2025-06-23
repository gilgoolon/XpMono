import React, { useState } from 'react';
import {
    Dialog, DialogTitle, DialogContent, DialogActions,
    TextField, Button, IconButton, Typography
} from '@mui/material';
import EditIcon from '@mui/icons-material/Edit';

export default function EditableLabel({ initialValue = "Click to edit", name = "Value", onSave }) {
    const [value, setValue] = useState(initialValue);
    const [editValue, setEditValue] = useState(initialValue);
    const [open, setOpen] = useState(false);

    const handleOpen = () => {
        setEditValue(value);
        setOpen(true);
    };

    const handleClose = () => setOpen(false);

    const handleSave = () => {
        if (onSave) onSave(editValue);
        setValue(editValue);
        setOpen(false);
    };

    return (
        <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
            <Typography variant="body1">{value || "None"}</Typography>
            <IconButton size="small" onClick={handleOpen}>
                <EditIcon fontSize="small" />
            </IconButton>

            <Dialog open={open} onClose={handleClose}>
                <DialogTitle>Edit {name}</DialogTitle>
                <DialogContent>
                    <TextField
                        fullWidth
                        variant="outlined"
                        value={editValue}
                        onChange={(e) => setEditValue(e.target.value)}
                        autoFocus
                    />
                </DialogContent>
                <DialogActions>
                    <Button onClick={handleClose}>Cancel</Button>
                    <Button onClick={handleSave} variant="contained">Save</Button>
                </DialogActions>
            </Dialog>
        </div>
    );
}
